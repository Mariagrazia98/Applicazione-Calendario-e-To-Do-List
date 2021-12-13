# Applicazione-Calendario-e-To-Do-List all-in-one

## Definizione del problema
CalDAV è uno standard aperto per la sincronizzazione di eventi di calendario e liste di task (o todo
list). La difficoltà nella gestione dello standard è l’adattabilità dei SW correntemente disponibili
a fornire un supporto integrato che permetta, ad esempio, di poter visualizzare in un unico posto
eventi (come le lezioni) e to-do (eventualmente congruentemente con date ed orari di scadenza). 
## Obiettivi
L’obiettivo primario del progetto è sviluppare una applicazione in grado di visualizzare ed interagire
con un server CalDAV (es: sabre/dav, Nextcloud, ecc), fornendo un’interfaccia grafica che
supporti l’integrazione di entrambe le parti (calendario e to-do list) nello stesso spazio. L’applicazione
potrà basarsi su una qualsiasi libreria GUI C++ di terze parti (es: QT, JUCE, FLTK, ecc.).
Unico vincolo insindacabile allo sviluppo del progetto è l’uso di CMake per tutta la catena di deployment.
Come obiettivi secondari, si richiede agli sviluppatori di porre la massima attenzione alle necessità
di asincronia e parallelizzazione del problema. A fronte, infatti, di una interazione utente con calendari
e task (creazione/modifica/cancellazione degli stessi) la natura del problema richiede la
necessità di sincronizzare le informazioni con il server al fine di supportare, tra le altre possibilità,
la condivisione del singolo calendario/task list tra più utenti.

## Tools utilizzati 
Per la realizzazione del server si è utilizzato sabre/dav. 
sabre/dav è il framework WebDAV più popolare per PHP, utilizzato per realizzare  server WebDAV, CalDAV e CardDAV.
sabre/dav supporta un vasto range di standard internet relativi ai suddetti protocolli.

Per la realizzazione dell’interfaccia grafica si è scelto il framework Qt.
Qt è un toolkit per creare interfacce grafiche cross-platform che possono essere eseguite su diversi hardware e sistemi operativi.  Non si limita solamente allo sviluppo di interfacce grafiche, ma permette anche di integrare funzionalità come la gestione di richieste HTTP verso e da un server, strutture dati generiche e multithreading.

Qt implementa un paradigma di programmazione event-based tramite l’utilizzo di funzioni definite come signal e slot che permettono la comunicazione tra oggetti/widget diversi. 
Un signal è emesso quando un particolare evento occorre.
Uno slot è una funzione che è chiamata in risposta a un particolare signal.

## Rappresentazione degli oggetti
CalDav utilizza il formato iCalendar per la rappresentazione di task ed eventi. Per rappresentarli in C++ abbiamo utilizzato la classe astratta CalendarObject che poi è stata estesa dalle classi CalendarEvent e CalendarToDo

### CalendarEvent (VEVENT)

UID

DTSTAMP

SUMMARY

LOCATION

DESCRIPTION

DTSTART

DTEND

EXDATE

RRLUE:FREQ=DAILY| WEEKLY | MONTHLY | YEARLY


### CalendarTodo (VTODO)

UID

DTSTAMP

SUMMARY

LOCATION

DESCRIPTION

PRIORITY

DTSTART

COMPLETED

EXDATE

RRLUE:FREQ=DAILY| WEEKLY | MONTHLY | YEARLY


## Funzionalità implementate
- Autenticazione
- Scelta della visualizzazione del/dei calendario/calendari
- Visualizzazione dei ICalendar objects giornalieri
- Creazione di un ICalendar Object
- Modifica di un ICalendar Object
- Eliminazione di un ICalendar Object
- Eliminazione di una ricorrenza di un ICalendar Object
- Gestione completamento del task
- Condivisione del calendario
- Sincronizzazione col server

Mariagrazia Paladino

Manuel Pepe

Adriana Provenzano
