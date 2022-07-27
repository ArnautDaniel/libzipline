#include "extern/zip.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>


void zip_walk(struct zip_t *zip, const char *path);

#define MAXPATH 4096

void zipline_recursive_descent_copy(const char* src, const char* dst){
  int len_src = strlen(src);
  int len_dst = strlen(dst);
  int max = len_src + len_dst + 255;
  char buf[max];

  // Sufficient for linux,  add windows version.
  snprintf(buf, max, "cp -R %s %s", src, dst);
  printf("%s", buf);
  system(buf);
  return;
}

void zipline_sign(){
  char buf[MAXPATH];
  snprintf(buf, MAXPATH, "gpg --detach-sign --default-key support@fluxinc.ca payload.zip");
  system(buf);
  return;
}

void zipline_delete(char* folder){
  char buf[MAXPATH];
  snprintf(buf, MAXPATH, "rm -rf %s", folder);
  system(buf);
  return;
}

char* zipline_gethash(){
  FILE *fp;
  char* path = malloc(sizeof(char)*MAXPATH);
  fp = popen("sha256sum payload.zip", "r");
  fgets(path, sizeof(char)*MAXPATH, fp);
  pclose(fp);
  for(int i = 0; i < MAXPATH; i++){
    if(path[i] == ' '){
      path[i] = '\0';
      break;
    }
  }
  return path;
}
  
void zipline_create(const char* pathname){

  char CWD[MAXPATH];
  getcwd(CWD, sizeof(CWD));
  
  char template[] = "/tmp/zipline-XXXXXXX";
  char* tempdir = mkdtemp(template);
  char payload_dir[MAXPATH];
  snprintf(payload_dir, MAXPATH, "%s/payload", tempdir);
  mkdir(payload_dir, 0700);
  zipline_recursive_descent_copy(pathname, payload_dir);

  chdir(tempdir);
  
  struct zip_t* zip = zip_open("payload.zip", ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
  zip_walk(zip, "payload");
  zip_close(zip);

  zipline_sign();
  zipline_delete("payload");
  char* hash = zipline_gethash();

  char finalfolder[MAXPATH];
  snprintf(finalfolder, MAXPATH, "zipline-%s", hash);
  mkdir(finalfolder, 0700);

  char finalcommand[MAXPATH];
  snprintf(finalcommand, MAXPATH, "cp payload.zip %s && cp payload.zip.sig %s", finalfolder, finalfolder);
  system(finalcommand);

  char finalfolderzip[MAXPATH];
  snprintf(finalfolderzip, MAXPATH, "%s.zip", finalfolder);
  struct zip_t* final_zip = zip_open(finalfolderzip, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');

  zip_walk(final_zip, finalfolder);
  zip_close(final_zip);
  
  return;
}

void zip_walk(struct zip_t *zip, const char *path) {
    DIR *dir;
    struct dirent *entry;
    char fullpath[MAX_PATH];
    struct stat s;

    memset(fullpath, 0, MAX_PATH);
    dir = opendir(path);
    assert(dir);

    while ((entry = readdir(dir))) {
      if (!strcmp(entry->d_name, ".\0") || !strcmp(entry->d_name, "..\0"))
        continue;

      snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
      stat(fullpath, &s);
      if (S_ISDIR(s.st_mode))
        zip_walk(zip, fullpath);
      else {
        zip_entry_open(zip, fullpath);
        zip_entry_fwrite(zip, fullpath);
        zip_entry_close(zip);
      }
    }
    closedir(dir);
}
