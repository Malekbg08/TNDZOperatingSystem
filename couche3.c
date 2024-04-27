#include <stdio.h>
#include <stdlib.h>
#include "os_defines.h"

void init_root(virtual_disk_t *vdisk){
  user_t root;
  char passwd[FILENAME_MAX_SIZE];
  char hashRes[SHA256_BLOCK_SIZE*2 + 1];
  printf("Choix mdp root: ");
  scanf("%s", passwd);
  fgetc(stdin);
  strcpy(root.login, "root");
  sha256ofString((BYTE *)passwd,hashRes); // hashRes contient maintenant le hash de l'item
  strcpy(root.passwd, hashRes);
  vdisk->users_table[ROOT_UID]=root;
  init_inode(vdisk, "password", sizeof(users_table_t),
             INODES_START+INODE_SIZE*INODE_TABLE_SIZE+1);
  strcpy(vdisk->inodes[vdisk->super_block.number_of_files-1].ctimestamp, timestamp());
  vdisk->inodes[vdisk->super_block.number_of_files-1].uright=rw;
  vdisk->inodes[vdisk->super_block.number_of_files-1].oright=rw;
  inc_first_free_byte(vdisk, sizeof(users_table_t));
  write_users_table(vdisk);
}

void add_user(virtual_disk_t *vdisk, char *login, char *passwd){
  user_t user;
  strcpy(user.login, login);
  char hashRes[SHA256_BLOCK_SIZE*2 + 1];
  sha256ofString((BYTE *)passwd,hashRes); // hashRes contient maintenant le hash de l'item
  strcpy(user.passwd, hashRes);
  vdisk->users_table[vdisk->super_block.number_of_users]=user;
  ++vdisk->super_block.number_of_users;
  printf("Utilisateur %s crée\n", login);
}

void rm_user(virtual_disk_t *vdisk, char *login, int userid){
  if (userid != ROOT_UID){
    printf("Seul root peut utiliser cette commande\n");
    return;
  }
  int uid=-1;
  for (uint i=1; i<vdisk->super_block.number_of_users; ++i){
    if (!strcmp(login, vdisk->users_table[i].login)){
      uid=i;
    }
  }
  if (uid>0){
    for (uint i=uid; i<vdisk->super_block.number_of_users-1; ++i){
      vdisk->users_table[i]=vdisk->users_table[i+1];
    }
    --vdisk->super_block.number_of_users;
    printf("Utilisateur supprimé\n");
  } else{
    printf("Utilisateur introuvalble\n");
  }
}

void write_users_table(virtual_disk_t *vdisk){
  fseek(vdisk->storage, INODES_START+INODE_SIZE*INODE_TABLE_SIZE*BLOCK_SIZE+1, SEEK_SET);
  fwrite(&vdisk->users_table, sizeof(users_table_t), 1, vdisk->storage);
}

int read_users_table(virtual_disk_t *vdisk){
  fseek(vdisk->storage, INODES_START+INODE_SIZE*INODE_TABLE_SIZE*BLOCK_SIZE+1, SEEK_SET);
  int retour = fread(&vdisk->users_table, sizeof(users_table_t), 1, vdisk->storage);
  if (retour < 1){
    return 1;
  }
  else{
    return 0;
  }
}
