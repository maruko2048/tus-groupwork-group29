#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define maxN 200
#define maxM 400
#define maxK 20
#define inf 1000000
#define time_limit 0.

long long dijkstraCount = 0;

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
void showAnswer(struct solution bestsolution, int M, int K, char *fname, struct edge_data edges[], int bestvalue);
void showValue(struct solution bestsolution, int M, int K, char *fname, struct edge_data edges[], int bestvalue);

/*ファイル名の取得とかデータの読み込みとかとりあえず書いてみたけど違ってたらごめん*/
// ↑一回やってみないとわからんね～ナイス
int main(void)
{
  clock_t start_t, end_t;
  double utime;

  /*変数一覧から持ってきたもの*/
  int i, j;                     // いつもの
  int N, M;                     // 頂点数, 辺数
  int v0, v1;                   // 始点, 終点を表す
  int k;                        // 消す辺の数
  int d[maxN], p[maxN];         // 始点から各頂点までの最短距離、最短経路木での親頂点を格納する
  int Lmat[maxN][maxN];         // 重み行列
  int edgeIdMat[maxN][maxN];    // 頂点u-v間の辺ID
  struct edge_data edges[maxM]; // 辺IDごとの辺情報
  struct solution bestsolution; // ベストな解
  int u, v, len;
  int bestvalue;
  char fname[128];                    /* 読み込むファイルの名前 */
  FILE *fp;                           /* 入力ファイル */
  printf("input filename: ");         /* ファイル名の入力を要求 */
  fgets(fname, sizeof(fname), stdin); /* 標準入力からファイル名を取得 */
  fname[strlen(fname) - 1] = '\0';    /* 最後の改行コードを除去 */
  fflush(stdin);                      /* 128 文字を超えた入力を標準入力から捨てる */
  fp = fopen(fname, "r");             /* ファイルを読み込みモードで開く */
  fscanf(fp, "%d %d", &N, &M);        /* ファイルから N, M を読み込む */
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
  // 処理開始
  start_t = clock();
  //
  // おためし
  bestsolution = greedy(N, k, Lmat, edgeIdMat, edges, v0, v1);
  end_t = clock();
  utime = (double)(end_t - start_t) / CLOCKS_PER_SEC;
  // showAnswer(bestsolution, M, k, fname, edges);
  showValue(bestsolution, M, k, fname, edges, bestvalue);
  printf("Dijkstra calls: %lld\n", dijkstraCount);
  printf("time: %f sec\n", utime);
  return 0;
}

/*ダイクストラ絡みゾーン2 始まり*/
int dijkstra(int N, int Lmat[maxN][maxN], int v0, int v1, int d[maxN], int p[maxN])
{
  dijkstraCount++;
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
  struct solution S;

  S.count = 0;

  for (int t = 0; t < K; t++)
  {
    // 現在の最短経路
    dijkstra(N, Lmat, v0, v1, d, p);

    int bestEdge = -1;
    int bestValue = -1;

    // 最短経路を逆向きにたどる
    int v = v1;

    while (v != v0)
    {
      int u = p[v];
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

// 解を表示する関数
void showAnswer(struct solution bestsolution, int M, int K, char *fname, struct edge_data edges[], int bestvalue)
{
  int i, j;
  printf("---Best solution---\n");
  printf("File Name: %s\n", fname);
  printf("Value: %d\n", bestsolution.value);
  printf("Best Value: %d\n", bestvalue);
  printf("Removed Edges: \n");
  // 辺IDに対してどのu,vの辺かを表示する
  for (i = 0; i < bestsolution.count; i++)
  {
    printf("u: %d, v: %d, w: %d\n", edges[bestsolution.edgeId[i]].u, edges[bestsolution.edgeId[i]].v, edges[bestsolution.edgeId[i]].w);
  }
}
// FileNameとValueだけ表示する関数
void showValue(struct solution bestsolution, int M, int K, char *fname, struct edge_data edges[], int bestvalue)
{
  printf("File Name: %s\n", fname);
  printf("Value: %d\n", bestsolution.value);
  printf("Best Value: %d\n", bestvalue);
}
