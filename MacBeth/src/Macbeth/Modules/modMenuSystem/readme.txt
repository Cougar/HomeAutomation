[12:10:29] <arune> tanken med menysystemsmodulen har ju från "början" varit att den ska låta andra moduler synas
[12:11:02] <arune> så själva menysystemsmodulen ska bara visa en lista med de moduler som vill visa sig
[12:11:07] <arune> kommer se ut typ:
[12:11:10] <arune> TVChart
[12:11:15] <arune> Mediaplayer
[12:11:22] <arune> Temperatures
[12:11:55] <arune> tidigare var min tanke att menysystemsmodulen skulle hantera play/stop/fastforward och all sån skit
[12:12:17] <arune> men det ska den inte, den ska bara hantera "pad"-knappar, alltså höger/vänster/upp/ner
[12:12:50] <arune> och sen skicka vidare dessa kommandon till den modul som man är "inne i" (det vill säga den modul man aktiverat vid grundmenyn)
[12:15:10] <arune> så den modul som är aktiverad får pad-knappskommandon från menysystemsmodulen och ska sen skicka en meny till menysystemsmodulen 
[12:17:58] <arune> "submodulerna" ska själva hantera play/stop/vaddetnukanvara
[12:19:24] <arune> dessa submoduler ska få nån aktiveringssignal från menysystemsmodulen så att de vet när man är inne och bläddrar i dem, sen kommer de skicka en avaktiveringssignal tillbaks till menysystemsmodulen när man "lämnar" den modulen med fjärren (det kan ju inte menysystemsmodulen veta)
[12:20:50] <virus_> aha, jo det låter ju vettigt
[12:23:12] <arune> det näst sista är jag inte helt säler på, att modulerna ska få aktivering, menysystemsmodulen ska kunna hamna i grundmenyn helt plötsligt, kanske om man inte bläddrar på en kvart typ, anyway, går man sen in i mediaspelarmodulen igen t.ex. så ska man fortsätta där man var
[12:23:29] <arune> kanske bara låter som jidder, jag har ju inte klart allt för mig
[12:24:02] <virus_> hehe, njä....jag förstår iaf
[12:24:49] <arune> mediaplayermodulen ska konfas så att man anger en lista till alla ställen där man har musik t.ex. så ska dessa ställen listas när man går in i denna modulen (dvs det ska inte vara en filbrowser som tidigare)
[12:25:15] <arune> så när man går in i mediaplayer ser det ut så här:
[12:25:17] <arune> musik
[12:25:20] <arune> film
[12:25:22] <arune> osv
[12:25:30] <arune> går man in i musik så ser det ut så här
[12:25:46] <arune> album (kommer peka på c:/mp3/album)
[12:26:08] <arune> karins musik (kommer peka på h:/mina dok/download)
[12:26:25] <arune> så listan kommer ha path och title typ
[12:27:15] <arune> men sån är ju helt upp till modulerna sen, bara man har ett ok interface mellan menysystemsmodulen och submodulerna
[12:30:00] <virus_> mm, submodulerna är typ bara små klasser som menymodulen laddar då
[12:30:28] <virus_> eller ska det vara egna macbethmoduler
[12:30:37] <arune> nja, jag tänker mig vanliga macbethmoduler, som t.ex. tvchart, men som innehåller lite kod för att prata med menysystemsmodulen
[12:30:44] <virus_> oki
[12:30:58] <arune> sen kommer menysystemsmodulen skicka menyn som ska visas vidare till smalldisplays och någon möjlighet att presentera på tvn

skippa "aktivera"-kommandot, byt mot ett kommando som ska göra att submodulen skickar nuvarande lista bara
"första gången" skickar den alltså sin egen grund-meny

menusys -> submeny
activate		(respond)
deactivare		(dont respond)
goto param		(param will be sent to menusys before)
menusys <- submeny
leftmenu		(submenu will deactivate)
linkmessage	param	()
statusmessage	()

[17:16:18] <arune> jag kom på lite fler saker med menysystemsmodulen 
[17:19:48] <arune> det ska ju kunna komma "meddelanden" till menysystemsmodulen
[17:19:59] <arune> precis som det kommer till displayen just nu
[17:20:57] <arune> två typer, en som är "statusmeddelanden"
[17:21:14] <arune> andra typen är "länkmeddelanden"
[17:22:11] <virus_> ok. vad är skillnaden?
[17:22:46] <arune> enda skillnaden är att man kan trycka play/aktivera på den senare typen och hamna i den modulen som skickade, i dess meny så ska man kunna läsa om det meddelanden eller vad det kan vara
[17:22:59] <arune> första typen är t.ex. temperaturer
[17:23:26] <arune> jag tänker mig att detta meddelande kommer framför eller vid sidan av den vanliga menyn
[17:23:37] <arune> och trycks "bort" med godtycklig knapp
[17:24:16] <arune> när det är den andra typen (t.ex. new mail) så försvinner den kanske bara med stop (eller med padknapparna?)
[17:24:35] <arune> trycker man däremot på aktivera/play så ska man ju då hamna i den modulen på "rätt plats"
[17:25:03] <arune> t.ex. har man fått mail så ska man få upp en lista med mailen och kunna välja ett och läsa
[17:26:21] <arune> för tvchart tänker jag mig att när man byter kanal så dyker det upp samma meddelande som det idag gör på displayen, fast man kan trycka play och hamna i tvtablåmenyn där man kan kolla de kanaler man har inställt
[17:26:38] <arune> (fast när man nu följer länkmeddelandet så ska man hamna i den kanalen)
[17:27:04] <virus_> aha, tror jag förstår
[17:29:36] <arune> en sak som ska till då mellan menysystemsmodul och submodul är att menysystemsmodul ska kunna aktivera/avaktivera submodulen, så inte den agerar på play osv
[17:30:58] <arune> något uppenbart jag missat?
[17:31:18] <arune> jag kom att tänka på detta när jag spånade att ha en stor lcddisplay
[17:31:45] <arune> då kommer ju det som är på tvn hamna på denna displayen


[17:47:26] <arune> där sa du något, macbeth ska kunna ligga på en dator som inte är samma som menysystemet och mediaspelaren är på
[17:48:19] <arune> kanske gör man menysystemsguit på godtyckligt sätt (troligtvis inte java?), något standalone iaf
[17:48:36] <arune> som sen menysystemsmoduelen skickar tcp-data till
[17:48:43] <virus_> går ju alltid göra som nån slags tcp-modul
[17:48:51] <arune> borde ju vara oneway-data typ
[17:49:14] <arune> jag ser liksom inte hur man kan göra ett bra gui i java, men det kanske går? :)
[17:49:32] <arune> plattformsoberoendet är iofs bra där också?
[17:50:57] <virus_> det går nog. skulle exempelvis kunna ha nån slags html-viewer bara vars kod genereras dynamiskt beroende på vad som ska visas. då blir ju bilder och sånt smidigt
[17:51:10] <virus_> om det är för slött så kan man säkerligen göra nåt enklare
[17:51:44] <arune> mmm, gillar idéen med html-viewer
[17:52:14] <arune> ska man ha macbeth som en slags webserver då och man ger bara viewern en adress?
[17:52:28] <virus_> hehe, skulle man också kunna ha
[17:52:29] <arune> så macbeth står för htmlkod och bilder
[17:52:33] <virus_> nog en bra idé
[17:52:35] <arune> hur menade du annars?
[17:52:40] <virus_> oklart=)
[17:52:43] <arune> hehe
[18:03:54] <arune> men oklart är kanske fortfarande hur submenyn ska kunna leverera bilder till menysysmodulen som sen serverar dem på en webserver
[18:04:50] <arune> sen ska det till en mediaspelare som man kan prata tcp med