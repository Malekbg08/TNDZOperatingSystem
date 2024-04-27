#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include "os_defines.h"

virtual_disk_t* init_disk_sos(char* nom_rep){
  virtual_disk_t *vdisk = malloc(sizeof(virtual_disk_t));
  strcat(nom_rep,"/");
  strcat(nom_rep,"d0");
  vdisk->storage=fopen(nom_rep, "r+b");
  if (vdisk == NULL){
    perror(nom_rep);
    exit(1);
  }
  read_users_table(vdisk);
  read_super_block(vdisk);
  read_inodes_table(vdisk);
  return vdisk;
}

uint compute_nblock(uint n){
  return n/BLOCK_SIZE+1;
}

void write_block(block_t block,int pos,virtual_disk_t *vdisk){
  vdisk->super_block.nb_blocks_used++;
  fseek(vdisk->storage, (long) pos*sizeof(block_t),SEEK_SET);
  fwrite(&block, sizeof(block_t),1,vdisk->storage);
}

int read_block(block_t *block, int pos, virtual_disk_t *vdisk){
  fseek(vdisk->storage, pos*sizeof(block_t), SEEK_SET);
  int b = fread(block, sizeof(block_t), 1, vdisk->storage);
  if (b < 1){
    return 1;
  }
  else{
    return 0;
  }
}

/*
//TEST COUCHE 1
int main(int argc, char *argv[]){
  virtual_disk_t *vdisk = init_disk_sos(argv[1]);
  block_t block, block2;
  strcpy((char*)block.data, "ana");
  write_block(block, 1, vdisk);
  printf("%d\n",read_block(&block2, 0, vdisk));
  printf("%s\n", block2.data);
}
*/
