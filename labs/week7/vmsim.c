// COMP1521 18s1 Week 07 Lab
// Virtual Memory Simulation

#include <stdlib.h>
#include <stdio.h>

typedef unsigned int uint;

// Page Table Entries

#define NotLoaded 0
#define Loaded    1
#define Modified  2

#define PAGESIZE  4096
#define PAGEBITS  12

#define actionName(A) (((A) == 'R') ? "read from" : "write to")

typedef struct {
   int status;        // Loaded or Modified or NotLoaded
   int frameNo;       // -1 if page not loaded
   int lastAccessed;  // -1 if never accessed
} PTE;

// Globals

uint nPages;         // how many process pages
uint nFrames;        // how many memory frames

PTE *PageTable;      // process page table
int *MemFrames;      // memory (each frame holds page #, or -1 if empty)

uint nLoads = 0;     // how many page loads
uint nSaves = 0;     // how many page writes (after modification)
uint nReplaces = 0;  // how many Page replacements

uint clock = 0;      // clock ticks

// Functions

void initPageTable();
void initMemFrames();
void showState();
int  physicalAddress(uint vAddr, char action);

// main:
// read memory references
// maintain VM data structures
// argv[1] = nPages, argv[2] = nFrames
// stdin contains lines of form
//   R Address
//   W Address
// R = read a byte, W = write a byte, Address = byte address
// Address is mapped to a page reference as per examples in lectures
// Note: pAddr is signed, because -ve used for errors

int main (int argc, char **argv)
{
   char line[100]; // line buffer
   char action;    // read or write
   uint vAddr;     // virtual address (unsigned)
   int  pAddr;     // physical address (signed)

   if (argc < 3) {
      fprintf(stderr, "Usage: %s #pages #frames < refFile\n", argv[0]);
      exit(1);
   }

   nPages = atoi(argv[1]);
   nFrames = atoi(argv[2]);
   // Value 2 also picks up invalid argv[x]
   if (nPages < 1 || nFrames < 1) {
      fprintf(stderr, "Invalid page or frame count\n");
      exit(1);
   }

   initPageTable(); initMemFrames();

   // read from standard input
   while (fgets(line,100,stdin) != NULL) {
      // get next line; check valid (barely)
      if ((sscanf(line, "%c %d\n", &action, &vAddr) != 2)
                     || !(action == 'R' || action == 'W')) {
         printf("Ignoring invalid instruction %s\n", line);
         continue;
      }
      // do address mapping
      pAddr = physicalAddress(vAddr, action);
      if (pAddr < 0) {
         printf("\nInvalid address %d\n", vAddr);
         exit(1);
      }
      // debugging ...
      printf("\n@ t=%d, %s pA=%d (vA=%d)\n",
             clock, actionName(action), pAddr, vAddr);
      // tick clock and show state
      showState();
      clock++;
   }
   printf("\n#loads = %d, #saves = %d, #replacements = %d\n", nLoads, nSaves, nReplaces);
   return 0;
}

// map virtual address to physical address
// handles regular references, page faults and invalid addresses
// vAddr = virtual address

int physicalAddress(uint vAddr, char action)
{

    uint page_number = vAddr / PAGESIZE;
    uint offset = vAddr % PAGESIZE;
    int physical_address = 0;
    
    // page number greater than number of pages (invalid page number)
    if (page_number >= nPages) {
        return -1;
    }
    // page already loaded
    if (PageTable[page_number].status == Loaded || PageTable[page_number].status == Modified) {
        if (action == 'W') {
            PageTable[page_number].status = Modified;
        }
        PageTable[page_number].lastAccessed = clock;
        physical_address = PageTable[page_number].frameNo * PAGESIZE + offset;            
    }
    // page not loaded, look for an unused frame
    else {
        int i = 0;
        int least_recent = 10000;
        int frame_number = -1;
        int found = 0;
        while (i < nFrames && found == 0) {
            if (MemFrames[i] == -1) {
               // use memframes[i]
               MemFrames[i] = page_number;
               frame_number = i;
               found = 1;
            }
            i++;
        }
        if (!(found)) {
        //no frame to use, need to replace a currently loaded frame
            nReplaces++;
            // looking for least recently loaded page
            i = 0;
            while (i < nPages) {
                if (PageTable[i].lastAccessed < least_recent && 
                PageTable[i].lastAccessed != -1) {
                    least_recent = PageTable[i].lastAccessed;
                    }
                i++;
            }              
            // now we have time of least recently loaded page, 
            // now we want the frame            
            i = 0;
            while (i < nPages) {
                if (PageTable[i].lastAccessed == least_recent) {
                    frame_number = PageTable[i].frameNo;
                    if (PageTable[i].status == Modified) {
                        nSaves++;
                    }
                    // kick out this page from the frame
                    PageTable[i].status = NotLoaded;
                    PageTable[i].frameNo = -1;
                    PageTable[i].lastAccessed = -1;
                    break;
                }
                i++;
            }           
        }
        // should now have a frame# to use
        nLoads++;
        // set PageTable entry for the new page
        // if we are writing, change status to modified
        if (action == 'W') {
            PageTable[page_number].status = Modified;
        } else {
            PageTable[page_number].status = Loaded;
        }
        PageTable[page_number].frameNo = frame_number;
        PageTable[page_number].lastAccessed = clock;
        // set memframes to pagenumber
        MemFrames[PageTable[page_number].frameNo] = page_number;
        physical_address = frame_number * PAGESIZE + offset;       
    }
    
   return physical_address; 
}

// allocate and initialise Page Table

void initPageTable()
{
   PageTable = malloc(nPages * sizeof(PTE));
   if (PageTable == NULL) {
      fprintf(stderr, "Insufficient memory for Page Table\n");
      exit(1);
   }
   for (int i = 0; i < nPages; i++) {
      PageTable[i].status = NotLoaded;
      PageTable[i].frameNo = -1;
      PageTable[i].lastAccessed = -1;
   }
}

// allocate and initialise Memory Frames

void initMemFrames()
{
   MemFrames = malloc(nFrames * sizeof(int));
   if (MemFrames == NULL) {
      fprintf(stderr, "Insufficient memory for Memory Frames\n");
      exit(1);
   }
   for (int i = 0; i < nFrames; i++) {
      MemFrames[i] = -1;
   }
}

// dump contents of PageTable and MemFrames

void showState()
{
   printf("\nPageTable (Stat,Acc,Frame)\n");
   for (int pno = 0; pno < nPages; pno++) {
      uint s; char stat;
      s = PageTable[pno].status;
      if (s == NotLoaded)
         stat = '-';
      else if (s & Modified)
         stat = 'M';
      else
         stat = 'L';
      int f = PageTable[pno].frameNo;
      printf("[%2d] %2c, %2d, %2d",
             pno, stat, PageTable[pno].lastAccessed, PageTable[pno].frameNo);
      if (f >= 0) printf(" @ %d", f*PAGESIZE);
      printf("\n");
   }
   printf("MemFrames\n");
   for (int fno = 0; fno < nFrames; fno++) {
      printf("[%2d] %2d @ %d\n", fno, MemFrames[fno], fno*PAGESIZE);
   }
}
