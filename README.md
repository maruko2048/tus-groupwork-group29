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
    START([START]) --> read[入力ファイルを読み込む]
    read --> timer[時間計測開始]
    timer --> greedy[貪欲法で初期解を生成]
    greedy --> initialize[初期解を最良解と現在解に設定]
    initialize --> firstLocal[初期解を局所探索]
    firstLocal --> initDisturb[disturbCountを1に設定]

    initDisturb --> resetCurrent[現在解を貪欲解に戻す]
    resetCurrent --> increment[disturbCountを1増やす]
    increment --> overLimit{disturbCountが10より大きい?}
    overLimit -- YES --> resetDisturb[disturbCountを2に戻す]
    overLimit -- NO --> disturb
    resetDisturb --> disturb[現在解の辺を<br/>disturbCount本入れ替える]

    disturb --> local[乱した現在解を局所探索し<br/>必要なら最良解を更新]
    local --> elapsed[経過時間を計算]
    elapsed --> timeOver{制限時間を超えた?}
    timeOver -- NO --> resetCurrent
    timeOver -- YES --> output[最良解・探索回数・実行時間を出力]
    output --> END([END])
```
