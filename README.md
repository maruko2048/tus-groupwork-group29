# プログラミング演習2 グループワーク

T=1.229s である。
コンピュータはだいたい1秒に10^8回計算できるので、オーダーが10^8を超えないように気を付ける。

# 命名規則

- 初めは小文字
- ローマ字読み使わない

# 関数一覧

```c
int dijkstra(int N, int Lmat[maxN][maxN], int v0, int v1, int d[maxN], int p[maxN])←一旦ヒープ使う方にした
(松村担当)
```

```c
void cutEdge(int Lmat[maxN][maxN], struct edge_data edges[maxM], int id){
    int u = edges[id].u;
    int v = edges[id].v;

    Lmat[u][v] = inf;
    Lmat[v][u] = inf;
}
```

```C
void restoreEdge(int Lmat[maxN][maxN], struct edge_data edges[maxM], int id)
{
  int u = edges[id].u;
  int v = edges[id].v;
  int w = edges[id].w;

  Lmat[u][v] = w;
  Lmat[v][u] = w;
}
//辺IDというのはstruct edgesの配列の何番目の要素かということ
```

```C
//完全に貪欲法でダイクストラ法を回す　消したedgeIDと長さと消した本数をsolutionに入れる
struct solution greedy(int N, int K, int Lmat[maxN][maxN], int edgeIdMat[maxN][maxN], struct edge_data edges[maxM], int v0, int v1)
{
  int d[maxN];
  int p[maxN];
  struct solution S;

  S.count = 0;

  for(int t=0;t<K;t++){
    // 現在の最短経路
    dijkstra(N,Lmat,v0,v1,d,p);

    int bestEdge = -1;
    int bestValue = -1;

    // 最短経路を逆向きにたどる
    int v = v1;

    while(v != v0){
      int u = p[v];
      int id = edgeIdMat[u][v];

      // 一時削除
      cutEdge(Lmat,edges,id);

      int value = dijkstra(N,Lmat,v0,v1,d,p);

      if(value > bestValue){
        bestValue = value;
        bestEdge = id;
      }

      restoreEdge(Lmat,edges,id);

      v = u;
    }

    if(bestEdge == -1)
      break;

    cutEdge(Lmat,edges,bestEdge);

    S.edgeId[S.count] = bestEdge;
    S.count++;
  }

  S.value = dijkstra(N,Lmat,v0,v1,d,p);

  return S;
}
```

グラフに対しダイクストラ法で最短経路の長さを求める。intで長さを返す。

- ダイクストラ法の関数
- 構成法(貪欲法,ランダム生成)の関数
- 改善法(多スタート局所探索法)の関数

# 変数一覧

## main外

```C
#define maxN 200
#define maxM 400
#define inf 1000000
```

```C
struct edge_data {
  int u; //この頂点から
  int v; //この頂点まで
  int w; //重み(辺の重みを戻すときこれを使う)
};

struct solution {
  int edgeId[maxK]; //消す辺のID
  int count; //今何個消しているか
  int value; //最短経路の長さ
};

struct cell { //ヒープ用
  int key;
  int vertex;
};
```

## main内

```C
int i,j //いつもの
int N, M; //頂点数, 辺数
int v0, v1; //始点, 終点を表す
int k; //消す辺の数
int d[maxN], p[maxN]; //始点から各頂点までの最短距離、最短経路木での親頂点を格納する
int Lmat[maxN][maxN];      // 重み行列
int edgeIdMat[maxN][maxN]; // 頂点u-v間の辺ID
struct edge edges[maxM];   // 辺IDごとの辺情報
struct solution bestsolution; //ベストな解

```

# 細かい仕様

- 初期解生成はSの切り落とす辺の重みを1000000にすることによって実現
- 初期状態のSと作業中のSと最良解のSをもつ

# フローチャート

以下にmain.cの(大まかな)フローチャートを示す。

```mermaid
%%{init:{'theme':'dark'}}%%
flowchart TD
    START([START]) --> read[入力を読む]
    read --> timer[時間計測開始]
    timer --> first[1回目の初期解生成]

    first --> greedy[完全貪欲法で初期解 S を作る]
    greedy --> evalS[S の評価値を計算]
    evalS --> setBest[best = S とする]

    setBest --> local[局所探索を開始]

    local --> makeNeighbor[近傍を調べる<br/>S から1本戻し<br/>最短路上の辺を1本削除]
    makeNeighbor --> better{より良い近傍解がある?}

    better -- YES --> updateS[S を更新]
    updateS --> local

    better -- NO --> localEnd[この初期解に対する<br/>局所探索終了]

    localEnd --> updateBest{best より良い?}
    updateBest -- YES --> saveBest[best を更新]
    updateBest -- NO --> checkTime[時間確認]
    saveBest --> checkTime

    checkTime --> timeLeft{まだ時間がある?}
    timeLeft -- YES --> randomInit[新しい初期解を生成]
    timeLeft -- NO --> output[best を出力]

    randomInit --> randomGreedy[ランダム化貪欲法で<br/>初期解 S を作る]
    randomGreedy --> local

    output --> END([END])
```

```mermaid
%%{init:{'theme':'dark'}}%%

```
