# Synchronizácia procesov

## Úvod

Tento projekt je inšpirovaný knihou "The Little Book of Semaphores" od Allena B. Downeyho a simulujú situáciu na pošte, kde sa synchronizujú procesy zákazníkov a úradníkov.

## Popis úlohy

Existujú 3 typy procesov: hlavný proces, poštový úradník a zákazník. Zákazníci prichádzajú na poštu s požiadavkami na listovné služby, balíky alebo peňažné služby a sú zaradení do fronty podľa typu služby. Úradníci obsluhujú fronty náhodne.

## Spustenie

Program sa spúšťa s parametrami pre počet zákazníkov, počet úradníkov, maximálnu čakaciu dobu zákazníka, maximálnu dĺžku prestávky úradníka a maximálny čas zatvorenia pošty.

## Implementačné detaily

- Na synchronizáciu procesov sa používajú zdieľaná pamäť a semafory.
- Kód je komentovaný a prehľadný.

## Preklad a odovzdanie

- Projekt sa implementuje v jazyku C.
- Pre preklad sa používa nástroj `make`.
- Odovzdávajú sa zdrojové kódy a súbor `Makefile`, zabalene do archívu `proj2.zip`.
- Projekt musí byť možné preložiť a spustiť na serveri merlin.

Toto je stručný popis zadania projektu 2. Pre viac detailov a implementačných pokynov sa pozrite do samotného zadania.

