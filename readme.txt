PC-9801シリーズ で S98 を鳴らすやーつ


【使い方】
　S98Play S98-File [/Paddr,data] [/Wtime]

　オプション:
　　　/P　　I/O ポートを指定します
　　　addr: アドレス ポート(16進数)
　　　data: データ ポート(16進数)
　　　　/P018c,018e で サウンド オーケストラ系のOPLが鳴るはず

　　　/W　　I/O アクセスのウェイト値を指定します
　　　time: ウエイト値(10進数)
　　　　未指定時は /W8 になります。


【仕様】
　・OPNA系/OPL系データの場合、1番目のチップを自動判定します
　　　/P オプションが指定されている場合は、そちらを利用します

　　　自動判定できるボード
　　　　　・PC-9801-86
　　　　　・PC-9801-118 (OPL3モード)
　　　　　・スピークボード (OPNA系)
　　　　　・SoundBlaster16 (OPL系)

　・OPNA系/OPL系以外の音源ではポートチェックをしないため、
　　必ず /P オプションでポートを指定してください

　　　例:
　　　・OPMなど
　　　　　/P888,88a

　　　・OPN2など
　　　　　/P188,18a,18c,18e

　　　・NBV4 などの複合型
　　　　　/P888,88a,,,1888,188a,,,2888,288a


　・S98のデバイス(チップ)は4つまで対応しています
　　　Pオプションの最大指定は以下のとおりです
　　　/Paddr1l,data1l,addr1h,data1h,addr2l,data2l,addr2h,data2h,addr3l,data3l,addr3h,data3h,addr4l,data4l,addr4h,data4h


