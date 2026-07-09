# プログラミング演習2 グループワーク
T=1.229s である。
コンピュータはだいたい1秒に10^8回計算できるので、オーダーが10^8を超えないように気を付ける。
# 関数一覧
```c
int Dijkstra(誰かここ書いて(´;ω;｀))←一旦やります(松)
```
グラフに対しダイクストラ法で最短経路の長さを求める。intで長さを返す。
```c
こういう風に書けばC記法でシンタックスハイライトされる
printf("hello world!")
```
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
