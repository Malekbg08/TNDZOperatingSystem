#include <stdio.h>
#include <stdlib.h>
#include "os_defines.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *timestamp(){
    time_t current_time;
    char* c_time_string;

    /* Obtain current time. */
    current_time = time(NULL);

    if (current_time == ((time_t)-1))
    {
        (void) fprintf(stderr, "Failure to obtain the current time.\n");
        exit(EXIT_FAILURE);
    }

    /* Convert to local time format. */
    c_time_string = ctime(&current_time);

    if (c_time_string == NULL)
    {
        (void) fprintf(stderr, "Failure to convert the current time.\n");
        exit(EXIT_FAILURE);
    }
    c_time_string[strlen(c_time_string)-1] = '\0';
    return c_time_string;
}

int write_super_block(virtual_disk_t *vdisk){
  fseek(vdisk->storage, 0,SEEK_SET);
  return fwrite(&vdisk->super_block, sizeof(super_block_t),1,vdisk->storage) == 1;
}

int read_super_block(virtual_disk_t *vdisk){
  fseek(vdisk->storage,0,SEEK_SET);
  int b = fread(&vdisk->super_block,sizeof(super_block_t),1,vdisk->storage);
  if (b < 1){
    return 1;
  }
  else{
    return 0;
  }
}

void inc_first_free_byte(virtual_disk_t *vdisk,int val){
  vdisk->super_block.first_free_byte+=val;
}

int read_inodes_table(virtual_disk_t *vdisk){
  fseek(vdisk->storage, INODES_START, SEEK_SET);
  int retour = fread(&vdisk->inodes, sizeof(inode_table_t), 1, vdisk->storage);
  if (retour < 1){
    return 1;
  }
  else{
    return 0;
  }
}

int write_inodes_table(virtual_disk_t *vdisk){
  fseek(vdisk->storage, INODES_START, SEEK_SET);
  return fwrite(&vdisk->inodes, sizeof(inode_table_t), 1, vdisk->storage) == 1;
}

void delete_inode(virtual_disk_t *vdisk, int pos){
  if (vdisk->super_block.number_of_files == 0) return;
  while(pos+1 < vdisk->super_block.number_of_files){
    vdisk->inodes[pos] = vdisk->inodes[pos+1];
    ++pos;
  }
  --vdisk->super_block.number_of_files;
}

int get_unused_inode(virtual_disk_t *vdisk){
  return vdisk->super_block.number_of_files;
}

void init_inode(virtual_disk_t *vdisk, char *filename, int size, int pos){
  int i = get_unused_inode(vdisk);
  ++vdisk->super_block.number_of_files;
  strcpy(vdisk->inodes[i].filename, filename);
  vdisk->inodes[i].size = size;
  vdisk->inodes[i].first_byte = pos;
  vdisk->inodes[i].nblock = (size+BLOCK_SIZE-1)/BLOCK_SIZE;
  strcpy(vdisk->inodes[i].mtimestamp, timestamp());
}


void cmd_dump_inode(char *rep){
  virtual_disk_t *vdisk = init_disk_sos(rep);
  vdisk->super_block.number_of_files=1;
  vdisk->super_block.number_of_users=2;
  vdisk->super_block.nb_blocks_used=3;
  vdisk->super_block.first_free_byte=INODES_START+INODE_SIZE*INODE_TABLE_SIZE+1;
  write_super_block(vdisk);
  write_inodes_table(vdisk);
  init_inode(vdisk, "fich1", 4, 3);
  init_inode(vdisk, "fich2", 4, 3);
  init_inode(vdisk, "fich3", 4, 3);
  delete_inode(vdisk, 0);

  write_super_block(vdisk);
  write_inodes_table(vdisk);
}
/*
int main(int argc, char *argv[]){
  if (argc != 2){
    fprintf(stderr, "Usage : %s dirname", argv[0]);
    exit(1);
  }
  cmd_dump_inode(argv[1]);
  return 0;
}*/
