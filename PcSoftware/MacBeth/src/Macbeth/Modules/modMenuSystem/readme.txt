[12:10:29] <arune> tanken med menysystemsmodulen har ju fr�n "b�rjan" varit att den ska l�ta andra moduler synas
[12:11:02] <arune> s� sj�lva menysystemsmodulen ska bara visa en lista med de moduler som vill visa sig
[12:11:07] <arune> kommer se ut typ:
[12:11:10] <arune> TVChart
[12:11:15] <arune> Mediaplayer
[12:11:22] <arune> Temperatures
[12:11:55] <arune> tidigare var min tanke att menysystemsmodulen skulle hantera play/stop/fastforward och all s�n skit
[12:12:17] <arune> men det ska den inte, den ska bara hantera "pad"-knappar, allts� h�ger/v�nster/upp/ner
[12:12:50] <arune> och sen skicka vidare dessa kommandon till den modul som man �r "inne i" (det vill s�ga den modul man aktiverat vid grundmenyn)
[12:15:10] <arune> s� den modul som �r aktiverad f�r pad-knappskommandon fr�n menysystemsmodulen och ska sen skicka en meny till menysystemsmodulen 
[12:17:58] <arune> "submodulerna" ska sj�lva hantera play/stop/vaddetnukanvara
[12:19:24] <arune> dessa submoduler ska f� n�n aktiveringssignal fr�n menysystemsmodulen s� att de vet n�r man �r inne och bl�ddrar i dem, sen kommer de skicka en avaktiveringssignal tillbaks till menysystemsmodulen n�r man "l�mnar" den modulen med fj�rren (det kan ju inte menysystemsmodulen veta)
[12:20:50] <virus_> aha, jo det l�ter ju vettigt
[12:23:12] <arune> det n�st sista �r jag inte helt s�ler p�, att modulerna ska f� aktivering, menysystemsmodulen ska kunna hamna i grundmenyn helt pl�tsligt, kanske om man inte bl�ddrar p� en kvart typ, anyway, g�r man sen in i mediaspelarmodulen igen t.ex. s� ska man forts�tta d�r man var
[12:23:29] <arune> kanske bara l�ter som jidder, jag har ju inte klart allt f�r mig
[12:24:02] <virus_> hehe, nj�....jag f�rst�r iaf
[12:24:49] <arune> mediaplayermodulen ska konfas s� att man anger en lista till alla st�llen d�r man har musik t.ex. s� ska dessa st�llen listas n�r man g�r in i denna modulen (dvs det ska inte vara en filbrowser som tidigare)
[12:25:15] <arune> s� n�r man g�r in i mediaplayer ser det ut s� h�r:
[12:25:17] <arune> musik
[12:25:20] <arune> film
[12:25:22] <arune> osv
[12:25:30] <arune> g�r man in i musik s� ser det ut s� h�r
[12:25:46] <arune> album (kommer peka p� c:/mp3/album)
[12:26:08] <arune> karins musik (kommer peka p� h:/mina dok/download)
[12:26:25] <arune> s� listan kommer ha path och title typ
[12:27:15] <arune> men s�n �r ju helt upp till modulerna sen, bara man har ett ok interface mellan menysystemsmodulen och submodulerna
[12:30:00] <virus_> mm, submodulerna �r typ bara sm� klasser som menymodulen laddar d�
[12:30:28] <virus_> eller ska det vara egna macbethmoduler
[12:30:37] <arune> nja, jag t�nker mig vanliga macbethmoduler, som t.ex. tvchart, men som inneh�ller lite kod f�r att prata med menysystemsmodulen
[12:30:44] <virus_> oki
[12:30:58] <arune> sen kommer menysystemsmodulen skicka menyn som ska visas vidare till smalldisplays och n�gon m�jlighet att presentera p� tvn

skippa "aktivera"-kommandot, byt mot ett kommando som ska g�ra att submodulen skickar nuvarande lista bara
"f�rsta g�ngen" skickar den allts� sin egen grund-meny

menusys -> submeny
activate		(respond)
deactivare		(dont respond)
goto param		(param will be sent to menusys before)
menusys <- submeny
leftmenu		(submenu will deactivate)
linkmessage	param	()
statusmessage	()

[17:16:18] <arune> jag kom p� lite fler saker med menysystemsmodulen 
[17:19:48] <arune> det ska ju kunna komma "meddelanden" till menysystemsmodulen
[17:19:59] <arune> precis som det kommer till displayen just nu
[17:20:57] <arune> tv� typer, en som �r "statusmeddelanden"
[17:21:14] <arune> andra typen �r "l�nkmeddelanden"
[17:22:11] <virus_> ok. vad �r skillnaden?
[17:22:46] <arune> enda skillnaden �r att man kan trycka play/aktivera p� den senare typen och hamna i den modulen som skickade, i dess meny s� ska man kunna l�sa om det meddelanden eller vad det kan vara
[17:22:59] <arune> f�rsta typen �r t.ex. temperaturer
[17:23:26] <arune> jag t�nker mig att detta meddelande kommer framf�r eller vid sidan av den vanliga menyn
[17:23:37] <arune> och trycks "bort" med godtycklig knapp
[17:24:16] <arune> n�r det �r den andra typen (t.ex. new mail) s� f�rsvinner den kanske bara med stop (eller med padknapparna?)
[17:24:35] <arune> trycker man d�remot p� aktivera/play s� ska man ju d� hamna i den modulen p� "r�tt plats"
[17:25:03] <arune> t.ex. har man f�tt mail s� ska man f� upp en lista med mailen och kunna v�lja ett och l�sa
[17:26:21] <arune> f�r tvchart t�nker jag mig att n�r man byter kanal s� dyker det upp samma meddelande som det idag g�r p� displayen, fast man kan trycka play och hamna i tvtabl�menyn d�r man kan kolla de kanaler man har inst�llt
[17:26:38] <arune> (fast n�r man nu f�ljer l�nkmeddelandet s� ska man hamna i den kanalen)
[17:27:04] <virus_> aha, tror jag f�rst�r
[17:29:36] <arune> en sak som ska till d� mellan menysystemsmodul och submodul �r att menysystemsmodul ska kunna aktivera/avaktivera submodulen, s� inte den agerar p� play osv
[17:30:58] <arune> n�got uppenbart jag missat?
[17:31:18] <arune> jag kom att t�nka p� detta n�r jag sp�nade att ha en stor lcddisplay
[17:31:45] <arune> d� kommer ju det som �r p� tvn hamna p� denna displayen


[17:47:26] <arune> d�r sa du n�got, macbeth ska kunna ligga p� en dator som inte �r samma som menysystemet och mediaspelaren �r p�
[17:48:19] <arune> kanske g�r man menysystemsguit p� godtyckligt s�tt (troligtvis inte java?), n�got standalone iaf
[17:48:36] <arune> som sen menysystemsmoduelen skickar tcp-data till
[17:48:43] <virus_> g�r ju alltid g�ra som n�n slags tcp-modul
[17:48:51] <arune> borde ju vara oneway-data typ
[17:49:14] <arune> jag ser liksom inte hur man kan g�ra ett bra gui i java, men det kanske g�r? :)
[17:49:32] <arune> plattformsoberoendet �r iofs bra d�r ocks�?
[17:50:57] <virus_> det g�r nog. skulle exempelvis kunna ha n�n slags html-viewer bara vars kod genereras dynamiskt beroende p� vad som ska visas. d� blir ju bilder och s�nt smidigt
[17:51:10] <virus_> om det �r f�r sl�tt s� kan man s�kerligen g�ra n�t enklare
[17:51:44] <arune> mmm, gillar id�en med html-viewer
[17:52:14] <arune> ska man ha macbeth som en slags webserver d� och man ger bara viewern en adress?
[17:52:28] <virus_> hehe, skulle man ocks� kunna ha
[17:52:29] <arune> s� macbeth st�r f�r htmlkod och bilder
[17:52:33] <virus_> nog en bra id�
[17:52:35] <arune> hur menade du annars?
[17:52:40] <virus_> oklart=)
[17:52:43] <arune> hehe
[18:03:54] <arune> men oklart �r kanske fortfarande hur submenyn ska kunna leverera bilder till menysysmodulen som sen serverar dem p� en webserver
[18:04:50] <arune> sen ska det till en mediaspelare som man kan prata tcp med