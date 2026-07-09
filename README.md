# プログラミング演習2 グループワーク
T=1.229s である。
コンピュータはだいたい1秒に10^8回計算できるので、オーダーが10^8を超えないように気を付ける。
# 命名規則
- 初めは小文字
- ローマ字読み使わない

# 関数一覧
```c
int dijkstra(int N, int Lmat[maxN][maxN], int v0, int d[maxN], int p[maxN])←一旦ヒープ使う方にした 
```
```c
void neighborhood
```
グラフに対しダイクストラ法で最短経路の長さを求める。intで長さを返す。  
・ダイクストラ法の関数  
・構成法(貪欲法,ランダム生成)の関数  
・改善法(多スタート局所探索法)の関数
# 変数一覧
```C
int N, M; //頂点数, 辺数
struct edge_data edge[maxM]; //辺データを表す変数
int W[maxM]; //辺の重みを格納する配列
int v0, v1; //始点, 終点を表す
int k; //消す辺の数
int d[maxN], p[maxN]; //始点から各頂点までの最短距離、最短経路木での親頂点を格納する
int 
```

```c
こういう風に書けばC記法でシンタックスハイライトされる
printf("hello world!")
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
