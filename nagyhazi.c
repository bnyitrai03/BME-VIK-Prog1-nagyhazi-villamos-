#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debugmalloc.h"

typedef struct
{   
    char valaszid;
    char valasz[500];
}answer;

typedef struct question
{
    int kerdesid;
    char kerdes[500];
    answer A;
    answer B;
    answer C;
    answer D;
    char helyesid;
    struct question *next;   
}question;

typedef struct game 
{
    int kerdesid;
    char valaszid;
    struct game *next;    
}game;

typedef struct player
{
    char nev[100];
    game *g_head;
    struct player *next;
    
}player;

/* 
input1.txt-ből beolvassa az adatokat a question listába
a kérdések és a válaszok hossza maximum 500 karakter lehet!
a kérdésazonosító nem lehet #0 !
*/
void read_to_question(question *head)
{
    FILE *f1;
    question *p = head;
    int kerdesid;
    f1 = fopen("input1.txt", "r");
    if (f1 == NULL)
    {
        printf("Hiba input1 megnyitásakor.");
        return;
    }

    while (fscanf(f1, "#%d\n", &kerdesid) == 1)
    {
      question *new = malloc(sizeof(question));
      new -> next = p -> next;
      p -> next = new;
      p = new;
      new -> kerdesid = kerdesid;
      fgets(new -> kerdes, 500, f1);
      fscanf(f1, "%c: ", &(new -> A.valaszid));
      fgets((new -> A.valasz), 500, f1);
      fscanf(f1, "%c: ", &(new -> B.valaszid));
      fgets((new -> B.valasz), 500, f1);
      fscanf(f1, "%c: ", &(new -> C.valaszid));
      fgets((new -> C.valasz), 500, f1);
      fscanf(f1, "%c: ", &(new -> D.valaszid));
      fgets((new -> D.valasz), 500, f1);
      fscanf(f1, "helyes valasz: %c\n", &(new -> helyesid));
    }

    if (fclose(f1) != 0)
    {
        printf("Hiba az input1.txt bezárásakor.");
        return;
    }
}

/* 
az input2.txt tartalmával feltölti a player - game fésűs listát
egy játékos neve maximum 100 karakter hosszú lehet!
*/
void read_to_player_and_game(player *head)
{
    FILE *f2;
    player *player_ptr = head;
    int kerdesid;
    char abcd;
    char nev[100];
    f2 = fopen("input2.txt", "r");
    if (f2 == NULL)
    {
        printf("Hiba input2 megnyitásakor.");
        return;
    }

    while(fgets(nev, 100, f2))
    {
        player *new = malloc(sizeof(player));
        game *g_ptr = NULL;
        new -> next = player_ptr -> next;                         // új játékost becsatoljuk a listába
        player_ptr -> next = new;
        player_ptr = new;
        new -> g_head = NULL;
        strcpy(new -> nev, nev);
        
        while(fscanf(f2, "#%d %c ", &kerdesid, &abcd) == 2)
        {
            game *gamehead = malloc(sizeof(game));
            gamehead -> next = g_ptr;                              // új játékot becsatoljuk a listába
            g_ptr = gamehead;
            gamehead -> kerdesid = kerdesid;
            gamehead -> valaszid = abcd;
            new -> g_head = gamehead;                              // létrehozzuk a fésűs kapcsolatot
        }

    }

     if (fclose(f2) != 0)
    {
        printf("Hiba az input2.txt bezárásakor.");
        return;
    } 
}
/*
megkapja a question lista fejét és a jelenlegi kérdés azonosítóját
visszatér az adott kérdés jó válaszának az azonosítójával
függvény jó működéséhez szükséges, hogy a keresett kerdesid benne legyen a question listában!
*/
char good_answer(question *head, int gameid)
{
    question *p = head -> next;
    for(; p -> kerdesid != gameid; p = p -> next)
    {}
    return p -> helyesid;
}

/*
bemenetként megkapja a q_id dinamikusan foglalt tömböt, és a vizsgálandó kérdés azonosítóját
megnézi hogy a kérdés benne van-e a q_id tömbben, ha igen akkor igaz értékkel tér vissza (1-el), ha nem akkor hamissal(0-al)
kerdesid nem lehet nulla, mert a calloc-tól kinullázott tömbként kapjuk meg a q_id -t, erre az estetre hibásan működik a need_to_test függvény!
*/
int need_to_test(int q_id[], int l, int kerdesid)
{
    int i;
    for ( i = 0; i < l; i++)
    {
        if (q_id[i] == kerdesid)
        return 0;   
    }
    return 1;   
}

/*
megkapja a question lista fejét, a jelenleg vizsgálandó player-re mutató pointert és azt a pointert ami megmutatja, hogy hol tartunk a jelenlegi player - game listájának vizsgálatában 
ha a vizsgált kérdésre a helytelen kérdés-válasz párok közül az egyik, meghaladta az 50%-ot, akkor visszaadja azt a válaszazonosítót, különben '0'-t ad vissza
*/
char check_current(question *q_head, player *p_current, game *g_current)
{
    
    player *p_ptr;
    game *g_ptr;
    char helyes;                                                                // kérdésre a helyesid-t tárolja
    int a = 0, b = 0, c = 0, d = 0;                                             // kérdésre hányan válaszoltak a,b,c,d -vel
    int sum = 0;                                                                // kérdésre összesen hányan válaszoltak 
    
    for(p_ptr = p_current; p_ptr -> next != NULL; p_ptr = p_ptr -> next)        // jelenlegi player -től bejárjuk a player listát
    {
        for(g_ptr = p_ptr -> g_head; g_ptr != NULL; g_ptr = g_ptr -> next)      // adott playernek bejárjuk a game listáját
        {      
            if(g_current -> kerdesid == g_ptr -> kerdesid)                       // ha a játékos ugyanazt a kérdést kapta, mint amit vizsgálunk
            {                                                                    // akkor növeljük azt, hogy hányan válaszoltak a kérdésre, eggyel, és azt az opciót is, amelyiket a játékos választott
                sum++;
                
                switch (g_ptr -> valaszid)
                {
                  case 'A': a++;
                  break;
                  case 'B': b++;
                  break;
                  case 'C': c++;
                  break;
                  case 'D': d++;
                  break;                  
                    
                }

            }

        }
    }

   helyes = good_answer(q_head, g_current -> kerdesid);                    // megnézzük, hogy a kérdésre melyik a jó válasz
   if (sum == 0)
   return '!';

   if(helyes != 'A' && (double)a / (double)sum > 0.5)                      // ha az A nem jó válasz, és több mint a játékosok fele A-val válaszolt, akkor visszatérünk vele, hogy kiírhassuk
   return 'A';
   if(helyes != 'B' && (double)b / (double)sum > 0.5)                      // ugyanez vonatkozik a B C D-re is
   return 'B';
   if(helyes != 'C' && (double)c / (double)sum > 0.5)
   return 'C';
   if(helyes != 'D' && (double)d / (double)sum > 0.5)
   return 'D';

   return '0';                                                              // ha nem született olyan eredmény, amit ki kell írni, akkor visszatérünk '0'-val
}

/*
bemenetként megkapja a question lista fejét
visszaadja a question lista által foglalt memóriát az operációs rendszernek
*/
void destroy_question(question *q_head)                       
{
    if(q_head == NULL)
    return;
    question *p = q_head;
    while(q_head != NULL)
    {
        p = q_head;
        q_head = q_head -> next; 
        free(p);
    }
}

/*
bemenetként megkapja a player lista fejét és hátsó strázsáját
visszaadja a player - game fésűs lista által foglalt memóriát az operációs rendszernek
*/
void destroy_player_and_game(player *p_head, player *p_tail)                
{
    int i = 0;
    player *p_seged = p_head;
    game *g_seged;
    game *g_delete;

    while(p_seged -> next -> next != NULL)                                    // player lista hátsó strázsájának a felszabadítása
    {
        p_seged = p_seged -> next;
    }
    p_seged -> next = NULL;                                                   // player lista farkát lecsatoljuk a listáról és felszabadítjuk
    free(p_tail);
    p_seged = p_head;                                                          

    while (p_head != NULL)
    {
        for ( g_seged = p_head -> g_head ;g_seged != NULL;)
        {
            if(i == 0)                                                      // player lista első strázsájának a felszabadítása
            {
                i = 1;
                break;
            }
            g_delete = g_seged;
            g_seged = g_seged -> next;
            free(g_delete);
        }
        p_seged = p_head;
        p_head = p_head -> next;
        free(p_seged);
    }   
}

int main()
{
    FILE *fout;
    char check;
    int l = 10;
    int i = 0;
    int *q_id = calloc(l, sizeof(int));                                                                            // a q_id dinamikusa foglalt tömbben tároljuk azoknak a kérdéseknek az azonosítóját, amiket már kiírtunk
    fout = fopen("output.txt", "w");                                                                               // output fájl megnyitása
    if (fout == NULL)
    {
        printf("Hiba az output.txt megnyitásakor");
        return 1;
    }

    question *q_head = malloc(sizeof(question));                                                                    // question lista létrehozása
    question *q_tail = malloc(sizeof(question));
    q_head -> next = q_tail;
    q_tail -> next = NULL;

    player *p_head = malloc(sizeof(player));                                                                        // player - game fésűs lista létrehozása
    player *p_tail = malloc(sizeof(player));
    p_head -> next = p_tail;
    p_tail -> next = NULL;

    read_to_question(q_head);                                                                                        // question lista feltöltése
    read_to_player_and_game(p_head);                                                                                 // player - game fésűs lista feltöltése

    for(player *p_current = p_head -> next; p_current -> next != NULL; p_current = p_current -> next)                // bejárjuk a player listát
    {
      for (game *g_current = p_current -> g_head; g_current != NULL; g_current = g_current -> next)                  // bejárjuk a game listát
      {
        if(need_to_test(q_id, l, g_current ->kerdesid))                                                               // megnézzük, hogy a jelenlegi kérdést megvizsgáltuk-e már
        {                                                                                                             
            if (i == l)                                                                                               // exponenciálisan növeljük a q_id méretét, ha nem elég nagy                                                                                       
            {
                l = l * 2;
                q_id = realloc(q_id, l);
            }
            q_id[i] = g_current -> kerdesid;                                                                           // betöltjük a q_id -be annak a kérdésnek az azonosítáját amit vizsgálni fogunk
            i++;
            check = check_current(q_head, p_current, g_current);                                                       // megnézzük azokat a helytelen kérdés-válasz párokat, melyek 50%-nál gyakrabban fordulnak elő
            if (check == '!')
            {
                printf("Hiba történt a keresés közben.");
                return 1;
            }
            else
            if (check != '0')
            fprintf(fout, "#%d %c\n", g_current -> kerdesid, check);                                                    // kiírjuk a kérdés azonosítót és a "kiszámolt" válaszopciót
        }
      }
      

    }
    
    free(q_id);                                                                                                          // felszabadítjuk az ideiglenes tömböt amiben a már megvizsgált kérdések azonosítóját tároltuk

    destroy_question(q_head);                                                                                            // felszabadítjuk a question listát
    destroy_player_and_game(p_head, p_tail);                                                                             // felszabadítjuk a player - game fésűs listát

    if (fclose(fout) != 0)                                                                                               // bezárjuk a kimenet fájlt
    {
        printf("Hiba az output.txt bezárásakor.");
        return 1;
    }

    return 0;
}