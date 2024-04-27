#include "os_defines.h"

int search_file(virtual_disk_t *vdisk, char *filename){
  for (uint i=0; i<vdisk->super_block.number_of_files; ++i){
    if (!strcmp(filename, vdisk->inodes[i].filename))
      return i;
  }
  return -1;
}

int write_file(virtual_disk_t *vdisk, char *filename, file_t *file){
  int i_file = search_file(vdisk, filename);
  if (i_file == -1){
    init_inode(vdisk, filename, file->size, vdisk->super_block.first_free_byte);
    strcpy(vdisk->inodes[vdisk->super_block.number_of_files-1].ctimestamp, timestamp());
    vdisk->inodes[vdisk->super_block.number_of_files-1].uright=RW;
    vdisk->inodes[vdisk->super_block.number_of_files-1].oright=Rw;
    fseek(vdisk->storage, vdisk->super_block.first_free_byte, SEEK_SET);
    fwrite(file->data, file->size, 1, vdisk->storage);
    inc_first_free_byte(vdisk, file->size);

  } else if(file->size <= vdisk->inodes[i_file].size){
    strcpy(vdisk->inodes[i_file].mtimestamp, timestamp());
    vdisk->inodes[i_file].size = file->size;
    fseek(vdisk->storage, vdisk->inodes[i_file].first_byte, SEEK_SET);
    fwrite(file->data, file->size, 1, vdisk->storage);

  } else{
    delete_inode(vdisk, i_file);
    init_inode(vdisk, filename, file->size, vdisk->super_block.first_free_byte);
    vdisk->inodes[vdisk->super_block.number_of_files-1].uright=RW;
    vdisk->inodes[vdisk->super_block.number_of_files-1].oright=Rw;
    fseek(vdisk->storage, vdisk->super_block.first_free_byte, SEEK_SET);
    fwrite(file->data, file->size, 1, vdisk->storage);
    inc_first_free_byte(vdisk, file->size);
  }
  write_inodes_table(vdisk);
  return 1;
}

int read_file(char* nom, file_t *file, virtual_disk_t* vdisk){
  for(uint i=0;i<vdisk->super_block.number_of_files;i++){
    if(!strcmp(vdisk->inodes[i].filename,nom)){
        fseek(vdisk->storage, vdisk->inodes[i].first_byte, SEEK_SET);
        fread(&(file->data), vdisk->inodes[i].size, 1, vdisk->storage);
        file->size = vdisk->inodes[i].size;
        return 1;
    }
  }
  return 0;
}

int delete_file(char* nom, virtual_disk_t* vdisk){
  for(uint i=0;i<vdisk->super_block.number_of_files;i++){
    if(!strcmp(vdisk->inodes[i].filename,nom)){
        printf("%s\n", nom);
        delete_inode(vdisk,i);
        return 1;
    }
  }
  return 0;
}

int load_file_from_host(virtual_disk_t *vdisk, char *filename){
  FILE* fd=fopen(filename, "rt");
  if (fd == NULL){
    perror(filename);
    return 0;
  }
  file_t file;
  struct stat infos;
  stat(filename, &infos);
  if (infos.st_size > MAX_FILE_SIZE){
    printf("Taille max de fichier depassée\n");
    return 0;
  }
  file.size = infos.st_size+1;
  int i=0;
  while(i<file.size-1){
    file.data[i]=fgetc(fd);
    ++i;
  }
  file.data[i]='\0';
  write_file(vdisk, filename, &file);
  fclose(fd);
  return 1;
}

int store_file_to_host(virtual_disk_t *vdisk, char *filename){
  file_t file;
  read_file(filename, &file, vdisk);
  FILE* fd=fopen(filename, "wt");
  fwrite(file.data, file.size-1, 1, fd);
  fclose(fd);
  return 1;
}
