#include "os_defines.h"

char* right_itoa(int right){
  if(right == rw) return "--";
  if(right == rW) return "-w";
  if(right == Rw) return "r-";
  if(right == RW) return "rw";
  return " ";
}

void ls(virtual_disk_t *vdisk, int l){
  if (l){
    for (uint i=0; i<vdisk->super_block.number_of_files; ++i){
        printf("  %s ", vdisk->inodes[i].filename);
        printf("%u octets owner:%u ", vdisk->inodes[i].size, vdisk->inodes[i].uid);
        printf("user:%s other:%s ", right_itoa(vdisk->inodes[i].uright),
                right_itoa(vdisk->inodes[i].oright));
        printf("%s %s ", vdisk->inodes[i].ctimestamp, vdisk->inodes[i].mtimestamp);
        printf("%u blocks\n", vdisk->inodes[i].nblock);
    }
  } else{
    for (uint i=0; i<vdisk->super_block.number_of_files; ++i){
        printf("  %s ", vdisk->inodes[i].filename);
        printf("%u octets\n", vdisk->inodes[i].size);
    }
  }
}

void cat(virtual_disk_t *vdisk, char *filename, int userid){
  int i;
  if ((i=search_file(vdisk, filename)) == -1){
    printf("Fichier %s inexistant\n", filename);
  } else{
    inode_t *inode = &(vdisk->inodes[i]);
    file_t file;
    if (userid == inode->uid){
      if (inode->uright >= Rw){
        read_file(filename, &file, vdisk);
        printf("%s\n", file.data);
      } else{
        printf("Vous n'avez pas le droit de lire ce fichier\n");
      }
    } else{
      if (inode->oright >= Rw){
        read_file(filename, &file, vdisk);
        printf("%s\n", file.data);
      } else{
        printf("Vous n'avez pas le droit de lire ce fichier\n");
      }
    }
  }
}

void rm(virtual_disk_t *vdisk,char *fichier,int userid)
{
  int i;
  if ((i=search_file(vdisk, fichier)) == -1){
    printf("Fichier %s inexistant\n", fichier);
  }
  else{
    inode_t *inode = &(vdisk->inodes[i]);
    if (userid == inode->uid){
          if(delete_file(fichier,vdisk)) printf("fichier supprimé\n");
          else printf("fichier non supprimé\n");
    }
    else{
      if (inode->oright == RW || inode->oright == rW)
      {
        if(delete_file(fichier,vdisk)) printf("fichier supprimé\n");
        else printf("fichier non supprimé\n");
      }
      else{
        printf("Vous n'avez pas le droit de supprimer  ce fichier\n");
      }
    }
  }
}

void cr(virtual_disk_t *vdisk, char *filename, int userid){
  if (vdisk->super_block.number_of_files >= INODE_TABLE_SIZE){
    printf("Nombre max de fichiers atteint\n");
    return;
  }
  file_t file;
  file.data[0]='\0';
  file.size=1;
  write_file(vdisk, filename, &file);
  vdisk->inodes[vdisk->super_block.number_of_files-1].uid=userid;
  vdisk->inodes[vdisk->super_block.number_of_files-1].uright=RW;
  vdisk->inodes[vdisk->super_block.number_of_files-1].oright=Rw;
  printf("Fichier crée\n");
}

void write_data(file_t *file){
  printf("Veuillez entrer le nouveau contenu (inferieur à %d octets):\n", MAX_FILE_SIZE);
  char string[MAX_FILE_SIZE];
  fgets(string, MAX_FILE_SIZE, stdin);
  while (strlen(string) > MAX_FILE_SIZE) {
    printf("Le contenu est superieur à la taille maximale du fichier\n");
    printf("Veuillez entrer le contenu à nouveau :\n");
    scanf("%s", string);
  }
  strcpy((char*)file->data, string);
  file->size = strlen((char*)file->data)+1;
}

void edit(virtual_disk_t *vdisk, char *filename, int userid){
  int i;
  if ((i=search_file(vdisk, filename)) == -1){
    printf("Fichier %s inexistant\n", filename);
  } else{
    inode_t *inode = &(vdisk->inodes[i]);
    file_t file;
    if (userid == inode->uid){
      if (inode->uright == rW || inode->uright == RW){
        write_data(&file);
        write_file(vdisk, filename, &file);
        printf("Fichier modifié\n");
      } else{
        printf("Vous n'avez pas le droit de modifier ce fichier\n");
      }
    } else{
      if (inode->oright == rW || inode->oright == RW){
        write_data(&file);
        write_file(vdisk, filename, &file);
        printf("Fichier modifié\n");
      } else{
        printf("Vous n'avez pas le droit de modifier ce fichier\n");
      }
    }
  }
}

void load(virtual_disk_t *vdisk,char *fichier){
  if (load_file_from_host(vdisk,fichier)){
    printf("Le fichier a bien été chargé\n");
  } else{
    printf("Le fichier n'a pas pu être chargé\n");
  }
}

void store(virtual_disk_t *vdisk, char *filename, int userid){
  if (search_file(vdisk, filename) == -1)
    printf("Fichier inexistant\n");
  else
    store_file_to_host(vdisk, filename);
}

void chown(virtual_disk_t *vdisk, char *filename, char *login,int userid){
  int i;
  if ((i = search_file(vdisk, filename)) == -1)
    printf("Fichier inexistant\n");
  else{
    inode_t *inode = &(vdisk->inodes[i]);
    if (userid == inode->uid || userid==0){
      uint new_uid=search_user(vdisk,login);
      if(new_uid!=-1){
         inode->uid=new_uid;
         printf("Le propriétaire a bien été modifié\n");
      } else{
        printf("le login n'existe pas\n");
      }
    } else{
      printf("Seul le propriétaire du fichier peut modifier le propriétaire du fichier\n");
    }
  }

}

int right_atoi(char *right){
  if(!strcmp(right, "rw")) return rw;
  if(!strcmp(right, "rW")) return rW;
  if(!strcmp(right, "Rw")) return Rw;
  if(!strcmp(right, "RW")) return RW;
  return -1;
}

void ch_mod(virtual_disk_t *vdisk, char *filename, int right, int userid){
  int i;
  if ((i = search_file(vdisk, filename)) == -1)
    printf("Fichier inexistant\n");
  else{
    inode_t *inode = &(vdisk->inodes[i]);
    if (userid == inode->uid){
      inode->oright = right;
      printf("Les droits ont bien été mis à jours\n");
    } else{
      printf("Seul le propriétaire du fichier peut modifier ses droits\n");
    }
  }
}

void listusers(virtual_disk_t *vdisk){
  for (uint i=0; i<vdisk->super_block.number_of_users; ++i){
    printf("  %s\n", vdisk->users_table[i].login);
  }
}

void quit(virtual_disk_t *vdisk){
  if (!write_super_block(vdisk)){
    printf("Erreur sauvegarde du superblock\n");
  }
  if (!write_inodes_table(vdisk)){
    printf("Erreur sauvegarde de la table d'inodes\n");
  }

  write_users_table(vdisk);
  fclose(vdisk->storage);
}

int search_user(virtual_disk_t *vdisk, char *login){
  for (uint i=0; i<vdisk->super_block.number_of_users; ++i){
    if (!strcmp(login, vdisk->users_table[i].login))
      return i;
  }
  return -1;
}

void choose_login(virtual_disk_t *vdisk, char *log){
  char login[FILENAME_MAX_SIZE];
  printf("Veuillez choisir un nom d'utilisateur : ");
  scanf("%s", login);
  while (search_user(vdisk, login) != -1 || strlen(login)>FILENAME_MAX_SIZE-1){
    if (strlen(login)>FILENAME_MAX_SIZE-1)
      printf("Le nom ne doit pas dépasser %d caractères\n", FILENAME_MAX_SIZE-1);
    else
      printf("Ce nom est déjà utlisé\n");
    printf("Veuillez choisir un nom d'utilisateur\n");
    scanf("%s", login);
  }
  strcpy(log, login);
}

void choose_passwd(virtual_disk_t *vdisk, char *passwd){
  printf("Veuillez choisir un mot de passe : ");
  scanf("%s", passwd);
  fgetc(stdin);
}

void new_user(virtual_disk_t *vdisk, int userid){
  if (userid != ROOT_UID){
    printf("Seul root peut utiliser cette commande\n");
    return;
  }
  if (vdisk->super_block.number_of_users >= NB_USERS){
    printf("Nombre maximal d'utilisateurs atteint\n");
    return;
  }
  char login[FILENAME_MAX_SIZE];
  char passwd[FILENAME_MAX_SIZE];
  choose_login(vdisk, login);
  choose_passwd(vdisk, passwd);
  add_user(vdisk, login, passwd);
  return;
}


void ui(char* la){
  int i=strlen(la);
  if(i>1)la[i-1]='\0';
}


void interprete(virtual_disk_t *vdisk,int userid){
  char cmd[100];
  printf("Terminal prêt\n");
  printf("%s$ ", vdisk->users_table[userid].login);
  fgets (cmd, 100, stdin);
  ui(cmd);
  while(strcmp(cmd,"quit")){
    char *token = strtok(cmd, " ");

    //printf("token : %s",token);

      if(!strcmp(token,"ls") ||!strcmp(cmd,"ls\n")){
        token=strtok(NULL, " ");
        if(token!=NULL && !strcmp(token,"-l"))
            ls(vdisk,1);

        else
            ls(vdisk,0);
      }
      else if(!strcmp(token,"cat") ||!strcmp(token,"cat\n")){
        token=strtok(NULL, " ");
        if(token!=NULL)
           cat(vdisk,token,userid);
        else{
           fprintf(stderr,"pas assez d'arguments\n");
        }
      }

      else if(!strcmp(token,"rm") ||!strcmp(token,"rm\n")){
        token=strtok(NULL, " ");
        if(token!=NULL)
           rm(vdisk,token,userid);
        else{
           fprintf(stderr,"pas assez d'arguments\n");
        }
      }
      else if(!strcmp(token,"cr") ||!strcmp(token,"cr\n")){
        token=strtok(NULL, " ");
        if(token!=NULL){
          cr(vdisk, token, userid);
        }else{
           fprintf(stderr,"pas assez d'arguments\n");
        }
      }

      else if(!strcmp(token,"edit\n") ||!strcmp(token,"edit")){
        token=strtok(NULL, " ");
        if(token!=NULL)
          edit(vdisk,token,userid);
        else{
           fprintf(stderr,"pas assez d'arguments\n");
        }
      }
      else if(!strcmp(token,"load\n") ||!strcmp(token,"load")){
        token=strtok(NULL, " ");
        if(token!=NULL)
          load(vdisk,token);
        else{
           fprintf(stderr,"pas assez d'arguments\n");
        }
      }
      else if(!strcmp(token,"listusers\n") ||!strcmp(token,"listusers")){
        listusers(vdisk);
      }

      else if(!strcmp(token,"store\n") ||!strcmp(token,"store")){
        token=strtok(NULL, " ");
        if(token!=NULL)
          store(vdisk,token,userid);
        else{
           fprintf(stderr,"pas assez d'arguments\n");
        }
      }
      else if(!strcmp(token,"chown\n") ||!strcmp(token,"chown")){
        token=strtok(NULL, " ");
        if(token!=NULL){
          char* token2=strtok(NULL, " ");
          if(token2!=NULL)
            chown(vdisk,token,token2,userid);
          else{
            fprintf(stderr,"pas assez d'arguments (login manquant)\n");
          }
        }
        else{
           fprintf(stderr,"pas assez d'arguments(login et fichier manquants)\n");
        }
      }

      else if(!strcmp(token,"chmod\n") ||!strcmp(token,"chmod")){
        token=strtok(NULL, " ");
        if(token!=NULL){
          char* token2=strtok(NULL, " ");
          if(token2!=NULL)
          ch_mod(vdisk, token,right_atoi(token2),userid);
          else{
            fprintf(stderr,"pas assez d'arguments (droits manquant)\n");
          }
        }
        else{
           fprintf(stderr,"pas assez d'arguments(droits et fichier manquants)\n");
        }
      }

      else if(!strcmp(token,"adduser") ||!strcmp(token,"adduser\n")){
        new_user(vdisk, userid);
      }

      else if(!strcmp(token,"rmuser") ||!strcmp(token,"rmuser\n")){
        token=strtok(NULL, " ");
        if(token!=NULL)
           rm_user(vdisk, token, userid);
        else{
           fprintf(stderr,"pas assez d'arguments\n");
        }
      }
    printf("%s$ ", vdisk->users_table[userid].login);
    fgets (cmd, 100, stdin);
    ui(cmd);
  }
  if(!strcmp(cmd,"quit")) quit(vdisk);

}

int verificationLogin(char* login, char* password,virtual_disk_t* vdisk){
  char hashPassword[SHA256_BLOCK_SIZE*2 + 1]; // contiendra le hash en hexadécimal
  sha256ofString((BYTE *)password,hashPassword); // hashRes contient maintenant le hash de l'item
  int indice=search_user(vdisk, login);
  if(indice!=-1){
    if(!strcmp(hashPassword,vdisk->users_table[indice].passwd))
      return 0;
  }
  return 1;
}

int main(int argc, char *argv[]){
  if (argc != 2){
    fprintf(stderr, "Usage: %s repertoire\n", argv[0]);
    exit(1);
  }
  virtual_disk_t *vdisk = init_disk_sos(argv[1]);
  char login[FILENAME_MAX_SIZE];
  char password[FILENAME_MAX_SIZE];
  int nbre_tentatives=1;
  printf("Entrez login: \n");
  scanf("%s",login);
  printf("Entrez mot de passe: \n");
  scanf("%s",password);
  fgetc(stdin);
  bool connexionreussie=false;
  while (nbre_tentatives<3){
    int temoin=nbre_tentatives;
    nbre_tentatives+=verificationLogin(login,password,vdisk);
    if(temoin==nbre_tentatives){
      connexionreussie=true;
      printf("\nConnexion réussie\n");
      int userid=search_user(vdisk, login);
      interprete(vdisk,userid);
      break;
    }
    else{
      printf("Incorrect, réessayez \n");
      printf("Entrez login: \n");
      scanf("%s",login);
      printf("Entrez mot de passe: \n");
      scanf("%s",password);
      fgetc(stdin);
    }

  }
  if(!connexionreussie){
    printf("\nIdentifiant / Mot de passe inconnus \n");
    printf("Fin de la session\n");
    return -1;
  }
  else return 1;
}
