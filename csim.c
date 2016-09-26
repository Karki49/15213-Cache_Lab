/* Name:  Aayush Karki 
 andnrew: aayushka*/

#include <stdio.h>
#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>

/*Tag validity*/
#define VALID 1
#define INVALID 0

    typedef unsigned long long Address_t;

    typedef enum {True, False} bool;

    typedef unsigned index_t;

    struct cmdline_t_parameters
    {
        unsigned hit;
        unsigned miss;
        unsigned evicts;
        short h, v, s, E, b;
        char *file;
    };
    struct cmdline_t_parameters param;

    typedef struct line_t{
        Address_t tag;
        unsigned age;/*counter for LRU. LRU line has larget count*/
        char valid; // 1 is valid; 0 is invalid
         
    } line_t;

    /*Set is an array of lines*/
    typedef struct Set{
        line_t *line;
    } set_t;


    /*Cache is an array of sets*/
    typedef struct Cache
    {
      set_t *set;  
    } Cache_t;

    Cache_t cache;

/*Function declaration*/
index_t get_AddrsIndex(Address_t addrs);
Address_t get_AddrsTag(Address_t addrs);
index_t getLRU_idx(index_t setIdx);
line_t newLine(char validity, Address_t tag, unsigned age);
void init_cache();
void evictLine(index_t setIdx, line_t line);
bool is_SetFull(index_t setIdx);
void erase_cache();
bool is_TagInSet(Address_t tag, index_t setIdx);
void simulate_cache(Address_t address);
void readTraces();
void displayHelp();
/*Function declarations end*/


/*returns set index of an address*/
index_t get_AddrsIndex(Address_t addrs){
    int tagbits = 64  - param.s - param.b;
    return (addrs << tagbits)>> (tagbits + param.b);
}


/*returns tag of an address*/
Address_t get_AddrsTag(Address_t addrs){
    return addrs >> (param.s + param.b);
}


/*get the index of the least recently used line from
a set whose index is given.*/
index_t getLRU_idx(index_t setIdx){
    index_t lineIdx = 0;
    int maxAge = 0;

    int i, age;
    for ( i = 0; i < param.E; ++i)
    {
        age = cache.set[setIdx].line[i].age;
        if (maxAge <= age ) {
            maxAge = age;
            lineIdx = i;
        }
    }
    return lineIdx;
    
}

/*return a new line with values initialized to parameters*/
line_t newLine(char validity, Address_t tag, unsigned age){
    line_t line; 
    line.valid = validity;
    line.tag = tag;
    line.age = age;
    return line;
}

/*create a new cache*/
void init_cache(){
    int i, j, E, S;
    S = 1<<param.s;
    E = param.E;
     
    line_t line;  

    cache.set = (set_t*)malloc(sizeof(set_t) * S);
    for ( i = 0; i < S; ++i) //itereate sets
    {
        line = newLine(INVALID, 0, 0);
        cache.set[i].line = (line_t*)malloc(sizeof(line_t)*E);

        /*add lines to a set*/
        for (j = 0; j < E; ++j)  cache.set[i].line[j] = line;
     
    }
    return;
}


/*Evict least recently used line from a set 
and replace it with given line.*/
void evictLine(index_t setIdx, line_t line){
    index_t lineIdx = getLRU_idx(setIdx);

    cache.set[setIdx].line[lineIdx] = line;    
    return;
}



/* Returns True is set(setIdx) is full. 
    Else returns False */
bool is_SetFull(index_t setIdx){
    int i = param.E - 1; //get the last set idx
    
    if (cache.set[setIdx].line[i].valid == VALID) return True;
    else {
        return False;}
}


/*Free the memory allocated for cache*/
void erase_cache(){
    int i,S;
    S = 1<<param.s;
    for (i = 0; i < S; ++i) free(cache.set[i].line); //erase lines
    free(cache.set); //erase sets
}


/*if a tag is in set (set idx), then return True.
  Else return False. */
bool is_TagInSet(Address_t tag, index_t setIdx){
    int i;
    bool is_TagInSet = False;
    
    for ( i = 0; i < param.E; ++i) //iterate lines in the set
    {
        cache.set[setIdx].line[i].age += 1;  //update age
        
            line_t *tmp = &cache.set[setIdx].line[i];
            /*check if a line is valid and if address tag matches
            line's tag. */
            if ( ( tmp->valid == VALID ) && (tmp->tag == tag) ) 
            {
                is_TagInSet = True; //True if tag found in set
                
                //line just accessed. Set age to 0
                cache.set[setIdx].line[i].age = 0; 
            }
        
    }
    return is_TagInSet;
}


/*Update hits, misses, evicts scores*/
void simulate_cache(Address_t address){

    index_t setIdx = get_AddrsIndex(address);
    Address_t tag = get_AddrsTag(address);


    if (is_TagInSet(tag, setIdx) == True){
        /*if tag is in set, then it's a hit*/
        param.hit +=1;
        /*If verbose flag is set, print hit*/
        if (param.v) printf(" %s", "hit");
        return;
    }  

    /*if not a hit, then it's a miss*/
    param.miss +=1;
    if (param.v) printf(" %s", "miss");
   
    line_t line = newLine(VALID, tag, 0); /*Line to be entered in set*/

    if (is_SetFull(setIdx) == True){ /*if set is full, evict a line */
        evictLine( setIdx, line);
        param.evicts +=1;
        if (param.v) printf(" %s", "eviction");
    }  
    else{
        /*if Set has space for line 
         add the line to the set*/       
        for (int i = 0; i < param.E; ++i)
        {
            if( cache.set[setIdx].line[i].valid == INVALID ){
                cache.set[setIdx].line[i] = line;   
                break;}
        }
    }
    return;
} 


/*Reads the valgrind output file traces*/
void readTraces(){
    FILE *fileName = fopen(param.file, "r");
    
    unsigned size;
    Address_t addrs;
    char oprt;

    /*read three values from a row in file*/
    while (fscanf(fileName, " %c %llx,%u", &oprt, &addrs, &size) == 3) {
       
       if (param.v==1) printf("\n%c %llx,%u", oprt, addrs, size);

       switch (oprt){
        case 'L':
            simulate_cache(addrs); 
            break;
        case 'S':
            simulate_cache(addrs); 
            break;
        case 'M':
            /*this operation is L followed by S*/
            simulate_cache(addrs); 
            simulate_cache(addrs);
        default:
            break;
       }

    }
    fclose(fileName);
    return ;
}



/*Prints help */
void displayHelp(){
  printf("%s","Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>");
  printf("%s\n", "Options:"); 
  printf("%s\n", "-h         Print this help message.");
  printf("%s\n", "-v         Optional verbose flag.");
  printf("%s\n", "-s <num>   Number of set index bits.");
  printf("%s\n", "-E <num>   Number of lines per set.");
  printf("%s\n", "-b <num>   Number of block offset bits.");
  printf("%s\n", "-t <file>  Trace file.");
  return;  
}


int main(int argc, char* argv[]) {

    int opt;

    /*Read command line arguments*/
    while( (opt=getopt(argc,argv,"[hv]s:E:b:t:")) >=0 ){
        switch (opt){
             case 'h':
                displayHelp();
                break;
            case 'v':
                param.v=1;
                break;
           case 's':
                param.s = atoi(optarg);
                break; 
            case 'E': 
                param.E = atoi(optarg);
                break;
            case 'b':
                param.b = atoi(optarg);
                break;
            case 't':
                param.file = optarg;
                break;
            default:
                displayHelp();
            break;
        }
    }

    param.hit = 0;
    param.miss = 0;
    param.evicts = 0;
    
    if (param.file != NULL){  
        init_cache(); //initialize the cache 
        //read the traces and simulate cache 
        readTraces();
        erase_cache();
    }

/*return the scores*/
printSummary(param.hit, param.miss, param.evicts);

return 0;

}
 
