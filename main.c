#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define maxN 200
#define maxM 400
#define maxK 20
#define inf 1000000

struct edge_data
{
  int u;
  int v;
  int w;
};

struct solution
{
  int edgeId[maxK];
  int count;
  int value;
};

/*ダイクストラ絡みゾーン1 始まり*/
struct cell /* 構造体 cell の定義 */
{
  int key;
  int vertex;
};
int parent(int i); /* parent 関数を宣言 */
int left(int i);
int right(int i);
void insert(struct cell *H, int *adr, int i, int a, int v);
void decrease_key(struct cell *H, int *adr, int i, int a);
int delete_min(struct cell *H, int *adr, int hsize);
void upheap_sort(struct cell *H, int *adr, int i);
void downheap_sort(struct cell *H, int *adr, int last);
int dijkstra(int N, int Lmat[maxN][maxN], int v0, int v1, int d[maxN], int p[maxN]);
/*ダイクストラ絡みゾーン1 終わり*/
void cutEdge(int Lmat[maxN][maxN], struct edge_data edges[maxM], int id);
void restoreEdge(int Lmat[maxN][maxN], struct edge_data edges[maxM], int id);
struct solution greedy(int N, int K, int Lmat[maxN][maxN], int edgeIdMat[maxN][maxN], struct edge_data edges[maxM], int v0, int v1);
struct solution disturbInitialSolution(int N, int M, int K, int Lmat[maxN][maxN], struct edge_data edges[maxM], int v0, int v1, struct solution *basesolution, int disturbCount);
// 局所探索ゾーン
void searchLocal(int N, int Lmat[maxN][maxN], int edgeIdMat[maxN][maxN], struct edge_data edges[maxM], int v0, int v1, struct solution *bestsolution, struct solution currentSolution, int goodNeighborCount); // 局所探索を行う関数
int makeNeighborAndTry(int N, int Lmat[maxN][maxN], struct edge_data edges[maxM], int v0, int v1, int id1, int id2);                                                                                           // 与えられたID1の辺を復元し、ID2の辺を削除し、評価し、更新をしたりしなかったりする
// ここまで
void showAnswer(struct solution bestsolution, char *fname, struct edge_data edges[], int bestvalue);
/*ファイル名の取得とかデータの読み込みとかとりあえず書いてみたけど違ってたらごめん*/
// ↑一回やってみないとわからんね～ナイス
// ファイル名が存在しないとか、そういう例外処理は今回「考えない」
int main(int argc, char *argv[])
{
  srand((unsigned)time(NULL));

  clock_t start_t, end_t;
  double utime;
  double time_limit;
  if (argc > 1)
  {
    time_limit = atof(argv[1]);
  }
  else
  {
    time_limit = 1; // デフォルトは1秒
  }

  /*変数一覧から持ってきたもの*/
  int i;                           // いつもの
  int N, M;                        // 頂点数, 辺数
  int v0, v1;                      // 始点, 終点を表す
  int k;                           // 消す辺の数
  int Lmat[maxN][maxN];            // 重み行列
  int edgeIdMat[maxN][maxN];       // 頂点u-v間の辺ID
  int goodNeighborCount = 5;       // 局所探索で見つけるより良い近傍解の数
  int maxDisturbCount = 10;        // 初期解を乱すときの辺の最大乱し数
  struct edge_data edges[maxM];    // 辺IDごとの辺情報
  struct solution bestsolution;    // ベストな解
  struct solution currentSolution; // 局所探索で使う
  struct solution greedysolution;  // 貪欲法で作った解

  int u, v, len;
  int bestvalue;
  char fname[128];                    /* 読み込むファイルの名前 */
  FILE *fp;                           /* 入力ファイル */
  printf("input filename: ");         /* ファイル名の入力を要求 */
  fgets(fname, sizeof(fname), stdin); /* 標準入力からファイル名を取得 */
  printf("\n");
  fname[strlen(fname) - 1] = '\0'; /* 最後の改行コードを除去 */
  fflush(stdin);                   /* 128 文字を超えた入力を標準入力から捨てる */
  fp = fopen(fname, "r");          /* ファイルを読み込みモードで開く */
  fscanf(fp, "%d %d", &N, &M);     /* ファイルから N, M を読み込む */
  if (N > maxN)
  {
    printf("N > maxN, redefine maxN\n");
  }
  for (u = 0; u < N; u++)
  {
    for (v = 0; v < N; v++)
    {
      Lmat[u][v] = inf;
    }
  }
  for (i = 0; i < M; i++)
  {
    fscanf(fp, " %d %d %d", &u, &v, &len);
    Lmat[u][v] = len;
    Lmat[v][u] = len;

    edges[i].u = u;
    edges[i].v = v;
    edges[i].w = len;

    edgeIdMat[u][v] = i;
    edgeIdMat[v][u] = i;
  }
  fscanf(fp, "%d", &v0);
  fscanf(fp, "%d", &v1);
  fscanf(fp, "%d", &k);
  fscanf(fp, "%d", &bestvalue);
  fclose(fp);
  // goodNeighborCountとmaxDisturbCountもコマンドライン引数で指定できるようにする　いろいろ試すため

  if (argc > 2)
  {
    goodNeighborCount = atoi(argv[2]);
  }
  if (argc > 3)
  {
    maxDisturbCount = atoi(argv[3]);
  }

  // 処理開始
  start_t = clock();
  greedysolution = greedy(N, k, Lmat, edgeIdMat, edges, v0, v1);                                     // 貪欲法で初期解を作る
  bestsolution = greedysolution;                                                                     // とりあえず貪欲法の解を最良としておく
  currentSolution = greedysolution;                                                                  // 局所探索で使う解を貪欲法の解で初期化する
  searchLocal(N, Lmat, edgeIdMat, edges, v0, v1, &bestsolution, currentSolution, goodNeighborCount); // 局所探索
  // disturbCountは2->3->...->maxDisturbCount->2->...と繰り返す 1は局所探索でやってるのとほぼ同じなので
  int disturbCount = 1; // 乱す辺の数
  while (1)
  {
    currentSolution = greedysolution;
    disturbCount++;
    if (disturbCount > maxDisturbCount)
    {
      disturbCount = 2;
    }
    currentSolution = disturbInitialSolution(N, M, k, Lmat, edges, v0, v1, &currentSolution, disturbCount); // 作業解をdisturbCountの数だけ乱す
    searchLocal(N, Lmat, edgeIdMat, edges, v0, v1, &bestsolution, currentSolution, goodNeighborCount);      // 生成した初期解に対して局所探索
    end_t = clock();
    utime = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    if (utime > time_limit)
    {
      break;
    }
  }
  showAnswer(bestsolution, fname, edges, bestvalue);

  printf("time: %f sec\n", utime);
  return 0;
}

/*ダイクストラ絡みゾーン2 始まり*/
int dijkstra(int N, int Lmat[maxN][maxN], int v0, int v1, int d[maxN], int p[maxN])
{
  struct cell Heap[maxN]; /* ヒープに用いる配列の宣言 */
  int adr[maxN];          /* ヒープに含まれる頂点のアドレスの配列 */
  int hsize;              /* ヒープに格納された頂点の数 */
  int w, u, v;
  for (u = 0; u < N; u++)
  {
    d[u] = inf;
    p[u] = -1;
    adr[u] = -1;
  }
  d[v0] = 0;
  hsize = 0;
  insert(Heap, adr, hsize, 0, v0);
  hsize++;
  while (hsize > 0)
  {
    v = delete_min(Heap, adr, hsize);
    hsize--;
    if (v == v1)
    {
      return d[v1];
    }
    for (w = 0; w < N; w++)
    {
      if (Lmat[v][w] == inf)
      {
        continue;
      }
      if (adr[w] == -1 && d[w] != inf)
      {
        continue;
      }
      if (d[w] == inf)
      {
        d[w] = d[v] + Lmat[v][w];
        p[w] = v;
        insert(Heap, adr, hsize, d[w], w);
        hsize++;
      }
      else if (d[v] + Lmat[v][w] < d[w])
      {
        d[w] = d[v] + Lmat[v][w];
        p[w] = v;
        decrease_key(Heap, adr, adr[w], d[w]);
      }
    }
  }
  return d[v1];
}
int parent(int i)
{
  return (i - 1) / 2;
}
int left(int i)
{
  return 2 * i + 1;
}
int right(int i)
{
  return 2 * i + 2;
}
void upheap_sort(struct cell *H, int *adr, int i)
{
  struct cell temp;
  int v1, v2;
  while (i > 0)
  {
    if (H[parent(i)].key <= H[i].key)
    {
      break;
    }
    v1 = H[i].vertex;
    v2 = H[parent(i)].vertex;
    temp = H[i];
    H[i] = H[parent(i)];
    H[parent(i)] = temp;

    adr[v1] = parent(i);
    adr[v2] = i;
    i = parent(i);
  }
}
void downheap_sort(struct cell *H, int *adr, int last)
{
  int i = 0;
  int c;
  struct cell temp;
  int v1, v2;
  while (left(i) <= last)
  {
    c = left(i);
    if (right(i) <= last && H[right(i)].key < H[left(i)].key)
    {
      c = right(i);
    }
    if (H[i].key <= H[c].key)
    {
      break;
    }
    v1 = H[i].vertex;
    v2 = H[c].vertex;
    temp = H[i];
    H[i] = H[c];
    H[c] = temp;

    adr[v1] = c;
    adr[v2] = i;
    i = c;
  }
}
void insert(struct cell *H, int *adr, int i, int a, int v)
{
  H[i].key = a;
  H[i].vertex = v;
  adr[v] = i;
  upheap_sort(H, adr, i);
}
void decrease_key(struct cell *H, int *adr, int i, int a)
{
  H[i].key = a;
  upheap_sort(H, adr, i);
}
int delete_min(struct cell *H, int *adr, int hsize)
{
  int v;
  v = H[0].vertex;
  adr[v] = -1;
  if (hsize > 1)
  {
    H[0] = H[hsize - 1];
    adr[H[0].vertex] = 0;
    downheap_sort(H, adr, hsize - 2);
  }
  return v;
}
/*ダイクストラ絡みゾーン2 終わり*/
void cutEdge(int Lmat[maxN][maxN], struct edge_data edges[], int id)
{
  int u = edges[id].u;
  int v = edges[id].v;

  Lmat[u][v] = inf;
  Lmat[v][u] = inf;
}
void restoreEdge(int Lmat[maxN][maxN], struct edge_data edges[], int id)
{
  int u = edges[id].u;
  int v = edges[id].v;
  int w = edges[id].w;

  Lmat[u][v] = w;
  Lmat[v][u] = w;
}
struct solution greedy(int N, int K, int Lmat[maxN][maxN], int edgeIdMat[maxN][maxN], struct edge_data edges[maxM], int v0, int v1)
{
  int d[maxN];
  int p[maxN];
  int temp_p[maxN];
  struct solution S;

  S.count = 0;

  for (int t = 0; t < K; t++)
  {
    // 現在の最短経路
    dijkstra(N, Lmat, v0, v1, d, p);
    for (int i = 0; i < N; i++)
    {
      temp_p[i] = p[i];
    }
    //

    int bestEdge = -1;
    int bestValue = -1;

    // 最短経路を逆向きにたどる
    int v = v1;

    while (v != v0)
    {
      int u = temp_p[v];
      int id = edgeIdMat[u][v];

      // 一時削除
      cutEdge(Lmat, edges, id);

      int value = dijkstra(N, Lmat, v0, v1, d, p);

      if (value > bestValue)
      {
        bestValue = value;
        bestEdge = id;
      }

      restoreEdge(Lmat, edges, id);

      v = u;
    }

    if (bestEdge == -1)
      break;

    cutEdge(Lmat, edges, bestEdge);

    S.edgeId[S.count] = bestEdge;
    S.count++;
  }

  S.value = dijkstra(N, Lmat, v0, v1, d, p);

  return S;
}
// goodNeighborCount個のより良い近傍解を見つけたら一番良いものに更新するようにする.
void searchLocal(int N, int Lmat[maxN][maxN], int edgeIdMat[maxN][maxN], struct edge_data edges[maxM], int v0, int v1, struct solution *bestsolution, struct solution currentSolution, int goodNeighborCount)
{
  int d[maxN], p[maxN], temp_p[maxN];
  int isImproved = 1;

  while (isImproved)
  {
    int goodNeighbor = 0;
    int bestValue = currentSolution.value;
    int bestOldId = -1;
    int bestNewId = -1;

    isImproved = 0;

    // 現在削除している辺を1本ずつ復元し、交換相手を探す
    for (int i = 0; i < currentSolution.count; i++)
    {
      int id1 = currentSolution.edgeId[i];

      // id1を復元したときの最短路を求める
      restoreEdge(Lmat, edges, id1);
      dijkstra(N, Lmat, v0, v1, d, p);

      // この後の評価でもp[]を使うので、上書きされる前に保存する
      for (int j = 0; j < N; j++)
      {
        temp_p[j] = p[j];
      }

      // いったん元のcurrentSolutionの状態に戻す
      cutEdge(Lmat, edges, id1);

      // id1を復元したことでできた最短路上の辺を交換候補にする
      int v = v1;
      while (v != v0)
      {
        int u = temp_p[v];
        int id2;
        int value;
        int alreadyDeleted = 0;

        if (u == -1)
        {
          break;
        }
        id2 = edgeIdMat[u][v];

        // すでに削除中の別の辺はid2にできない
        for (int j = 0; j < currentSolution.count; j++)
        {
          if (currentSolution.edgeId[j] == id2)
          {
            alreadyDeleted = 1;
            break;
          }
        }

        if (id1 != id2 && !alreadyDeleted)
        {
          // 　評価後にcurrentSolutionへ戻す
          value = makeNeighborAndTry(N, Lmat, edges, v0, v1, id1, id2);

          if (value > currentSolution.value)
          {
            goodNeighbor++;
            if (value > bestValue)
            {
              bestValue = value;
              bestOldId = id1;
              bestNewId = id2;
            }
          }
        }

        if (goodNeighbor >= goodNeighborCount)
        {
          break;
        }
        v = u;
      }

      if (goodNeighbor >= goodNeighborCount)
      {
        break;
      }
    }

    // 改善近傍がなければ局所最適なので終了
    if (bestOldId == -1)
    {
      break;
    }

    // 見つけた改善近傍のうち、一番良い交換だけを確定する
    restoreEdge(Lmat, edges, bestOldId);
    cutEdge(Lmat, edges, bestNewId);
    for (int i = 0; i < currentSolution.count; i++)
    {
      if (currentSolution.edgeId[i] == bestOldId)
      {
        currentSolution.edgeId[i] = bestNewId;
        break;
      }
    }
    currentSolution.value = bestValue;
    isImproved = 1;

    if (currentSolution.value > bestsolution->value)
    {
      *bestsolution = currentSolution;
    }
  }
}
// 1つの辺を復元し、1つの辺を削除することで近傍解を作り、評価する関数 評価値を返す。更新はしない。
int makeNeighborAndTry(int N, int Lmat[maxN][maxN], struct edge_data edges[maxM], int v0, int v1, int id1, int id2)
{
  // id1の辺を復元
  restoreEdge(Lmat, edges, id1);
  // id2の辺を削除
  cutEdge(Lmat, edges, id2);

  // 評価
  int d[maxN];
  int p[maxN];
  int value = dijkstra(N, Lmat, v0, v1, d, p);

  // 元に戻す
  cutEdge(Lmat, edges, id1);
  restoreEdge(Lmat, edges, id2);
  return value;
}
// 初期解を乱す関数　disturbCount個の辺を復元し、同じ数だけ削除することで近傍解を作る
struct solution disturbInitialSolution(int N, int M, int K, int Lmat[maxN][maxN], struct edge_data edges[maxM], int v0, int v1, struct solution *basesolution, int disturbCount)
{
  struct solution S = *basesolution;

  int positions[maxK];
  int candidates[maxM];
  int candidateCount = 0;
  int d[maxN], p[maxN];

  // Lmatとbasesolutionが同じでないと困るので、戻します
  for (int id = 0; id < M; id++)
  {
    restoreEdge(Lmat, edges, id);
  }

  // basesolutionの切断状態をLmat上に再現する
  for (int i = 0; i < basesolution->count; i++)
  {
    cutEdge(Lmat, edges, basesolution->edgeId[i]);
  }

  // disturbCountを都合よくする
  if (disturbCount > K)
  {
    disturbCount = K;
  }

  if (disturbCount > M - K)
  {
    disturbCount = M - K;
  }

  // Solution->EdgeIdの添え字のための配列
  for (int i = 0; i < K; i++)
  {
    positions[i] = i;
  }

  // positionsをシャッフルする
  // これはFisher-Yatesのシャッフルアルゴリズム
  // 先頭disturbCount個の位置を交換対象とする
  for (int i = K - 1; i > 0; i--)
  {
    int j = rand() % (i + 1);
    int temp = positions[i];
    positions[i] = positions[j];
    positions[j] = temp;
  }
  // candidates(切断候補)を作るための準備。すでに切断されている辺は除外する。
  for (int id = 0; id < M; id++)
  {
    int isCut = 0;

    for (int i = 0; i < K; i++)
    {
      if (basesolution->edgeId[i] == id)
      {
        isCut = 1;
        break;
      }
    }

    if (!isCut)
    {
      candidates[candidateCount] = id;
      candidateCount++;
    }
  }
  // candidatesをシャッフルする
  // 先頭disturbCount個を新しく切断する辺とする
  for (int i = candidateCount - 1; i > 0; i--)
  {
    int j = rand() % (i + 1);
    int temp = candidates[i];
    candidates[i] = candidates[j];
    candidates[j] = temp;
  }

  // disturbCount本の辺を入れ替える
  for (int i = 0; i < disturbCount; i++)
  {
    int position = positions[i];

    int restoreId = S.edgeId[position];
    int cutId = candidates[i];

    // 元々cutされていた辺を復元する
    restoreEdge(Lmat, edges, restoreId);

    // 代わりの辺をcutする
    cutEdge(Lmat, edges, cutId);

    // solution側の記録も変更する
    S.edgeId[position] = cutId;
  }

  // 乱した解の評価値を計算する
  S.value = dijkstra(N, Lmat, v0, v1, d, p);

  return S;
}
// 解を表示する関数
void showAnswer(struct solution bestsolution, char *fname, struct edge_data edges[], int bestvalue)
{
  int i;
  printf("File Name: %s\n", fname);
  printf("Value: %d\n", bestsolution.value);
  printf("Best Value: %d\n", bestvalue);
  printf("Removed Edges: \n");
  // 辺IDに対してどのu,vの辺かを表示する
  for (i = 0; i < bestsolution.count; i++)
  {
    printf("(%3d, %3d) w: %d\n", edges[bestsolution.edgeId[i]].u, edges[bestsolution.edgeId[i]].v, edges[bestsolution.edgeId[i]].w);
  }
}