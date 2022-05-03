# Chinese_chess
Terminal Chinese_chess

## 显示
执行之后，终端打印如下：
![初始效果](https://github.com/luoshmg/Chinese_chess/blob/main/img/chess_step0.png)

**GAME BEGINE之下是棋盘，视角为红方（棋子用大写字母表示）；**

**RED's turn: 等待输入红方（红方先行）的走法，例如F2t5，表示炮二平五。执行效果如下：**
![炮二平五](https://github.com/luoshmg/Chinese_chess/blob/main/img/chess_step1.png)

**类似的，BLACK's turn: 等待输入黑方的走法，例如d8a7，表示馬八进七。**

## 棋子、走法和输赢
1. 棋子：红方为ABCDEFG（帥仕相傌俥炮兵），黑方为abcdefg（將士象馬車砲卒）
2. 走法：（art表示进退平，fb表示前后）
    1. 相同棋子不在同一列时，走法如炮二平五（F2a5）、馬八进七（d8a7）等
    2. 相同棋子在同一列时，走法如后馬退三（bdr3）、前炮平六（fFa6）等
3. 输赢：
    1. 当一方帥或將阵亡时，对方赢。（不是一方将军把对方将死时赢）
    2. 当一方投降（命令为/fff）时，对方赢