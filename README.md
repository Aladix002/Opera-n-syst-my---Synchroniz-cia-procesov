# Projekt 2 - Synchronizace procesů

## Úvod

Tento projekt je inspirován knihou "The Little Book of Semaphores" od Allena B. Downeyho a simuluje situaci v poště, kde se synchronizují procesy zákazníků a úředníků.

## Popis úlohy

Existují 3 typy procesů: hlavní proces, poštovní úředník a zákazník. Zákazníci přicházejí na poštu s požadavky na listovní služby, balíky nebo peněžní služby, a jsou zařazeni do fronty podle typu služby. Úředníci obsluhují fronty náhodně. 

## Spuštění

Program se spouští s parametry pro počet zákazníků, počet úředníků, maximální čekací dobu zákazníka, maximální délku přestávky úředníka a maximální čas uzavření pošty.

## Implementační detaily

- Pro synchronizaci procesů se používají sdílená paměť a semafory.
- Kód je komentován a přehledný.

## Překlad a odevzdání

- Projekt se implementuje v jazyce C.
- Pro překlad se používá nástroj `make`.
- Odevzdávají se zdrojové kódy a soubor `Makefile`, zabaleny do archivu `proj2.zip`.
- Projekt musí být možné přeložit a spustit na serveru merlin.

Toto je stručný popis zadání projektu 2. Pro více detailů a implementačních pokynů se podívejte do samotného zadání.
