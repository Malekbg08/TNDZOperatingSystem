/// \file test_sha.c
/// \brief fichier pour montrer comment on utilise la fonction sha256ofString
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha256.h"
#include "sha256_utils.h"
#define STRLONG 30

int main(){
	char hashRes[SHA256_BLOCK_SIZE*2 + 1]; // contiendra le hash en hexadécimal
	char * item = malloc(50*sizeof(char)); // contiendra la chaîne à hasher
	strcpy(item,"L'éléphant lançait des avions en papier 1 2 3 123 !"); // c'est elle
	sha256ofString((BYTE *)item,hashRes); // hashRes contient maintenant le hash de l'item
	int long_hash = strlen(hashRes); // sa longueur en caractères hexadécimaux
	printf("Le sha : %s avec %d caractères hexadécimaux, soit %d octets comme prévu\n",hashRes,long_hash,long_hash/2);
	return 0;
}
