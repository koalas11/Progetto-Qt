# Introduzione
Il progetto richiede la progettazione e realizzazione di un’applicazione con GUI
per la gestione dei dipinti presenti nella Galleria degli Uffizi.
# Strumenti utilizzati
Qt: Qt Creator 4.15.0 <br>
Based on Qt 5.15.2 (GCC 7.3.1 20180303 (Red Hat 7.3.1-5), 64 bit) <br>

![alt text](https://github.com/koalas11/Progetto-Qt/blob/main/Screenshots/startScreen.PNG)
![alt text](https://github.com/koalas11/Progetto-Qt/blob/main/Screenshots/FilteredScreen.PNG)

## Scelte implementative e di design importanti
Uno dei problemi che ho riscontrato è la gestione delle date da inserire nel grafico essendo formattate in modo diverso e avendo troppi dati distinti, 
per questo ho decido di decidere una formattazione unica per le date prima di inserirle nel grafico cercando il primo numero presente nel campo data e 
dividendolo in secoli e metà secoli nel caso di numeri di 4, altrimenti viene mostrato il numero senza dividerlo in secoli e nel caso non sia presenta nessun numero viene mostrato “Data Non Numerica”. <br>
Per i grafici per la visione delle percentuali dei dipinti rispetto alla scuola e il numero di dipinti rispetto alla data ho deciso di usare due grafici a torta questo 
perché permettono di visualizzare meglio il rapporto di una categoria rispetto alle altre, un problema riscontrato usando questo tipo di grafico è la grande quantità di categorie presenti.<br>
Per ovviare a questo problema ho deciso di mostrare nella leggenda la categoria e la percentuale/numero di dipinti annessa, poi ho deciso anche di troncare le percentuali nella leggenda per 
renderle più visibili, nel caso si volesse la percentuale precisa tenendo il mouse sopra un pezzo della torta verrà mostrata la percentuale precisa.<br>
Visto che i grafici prendono molto spazio per essere leggibili ho deciso di mostrarne uno alla volta con la possibilità di cambiare da un grafico all’altro con il bottone “Cambia grafico”<br>
