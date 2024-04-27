#include "os_defines.h"

void init_super_block(virtual_disk_t *vdisk){
  super_block_t *superblock = malloc(sizeof(super_block_t));
  superblock->number_of_files=0;
  superblock->number_of_users=1;
  superblock->nb_blocks_used=SUPER_BLOCK_SIZE;
  superblock->first_free_byte=INODES_START+sizeof(inode_table_t)+1;
  fseek(vdisk->storage, 0,SEEK_SET);
  fwrite(superblock, SUPER_BLOCK_SIZE*BLOCK_SIZE,1,vdisk->storage);
  free(superblock);
}

void init_inodes_table(virtual_disk_t *vdisk){
  inode_table_t *inodes=malloc(sizeof(inode_table_t));
  fseek(vdisk->storage, INODES_START, SEEK_SET);
  fwrite(inodes, sizeof(inode_table_t), 1, vdisk->storage);
  free(inodes);
}

int main(int argc, char *argv[]){
  if (argc != 2){
    fprintf(stderr, "Usage: %s repertoire\n", argv[0]);
    exit(1);
  }
  virtual_disk_t *vdisk = init_disk_sos(argv[1]);
  init_super_block(vdisk);
  init_inodes_table(vdisk);
  read_super_block(vdisk);
  read_inodes_table(vdisk);
  init_root(vdisk);
  quit(vdisk);
  printf("Systeme correctement installé\n");
  return 0;
}
