#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

const char  *version_semver = "%%semver%% 0.0.2";
const int   offset_semver   = 11;
const int   max_semver_sz   = 256;
extern int  errno;

static
void *memmem(const void *haystack, size_t haystack_len, const void *needle, size_t needle_len) {
	const char *begin = haystack;
	const char *last_possible = begin + haystack_len - needle_len;
	const char *tail = needle;
	char point;

	/* The first occurrence of the empty string is deemed to occur at
	 * the beginning of the string. */
	if (needle_len == 0)
		return (void *)begin;

	/* Sanity check, otherwise the loop might search through the whole
	 * memory. */
	if (haystack_len < needle_len)
		return NULL;

	point = *tail++;
	for (; begin <= last_possible; begin++) {
		if (*begin == point && !memcmp(begin + 1, tail, needle_len - 1))
			return (void *)begin;
	}

	return NULL;
}

static void error(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}

static char * search_semver(const char * fname) {
  char        *buf;
  char        *search;
  char        *ret    = NULL;
  struct stat sb;
  int         fd;
  char        semver_atom[16] = {0};  
  
  // need to this to avoid matching to this semver string
  strcat(semver_atom, "%%");
  strcat(semver_atom, "semver");
  strcat(semver_atom, "%%");
  
  if((fd = open(fname, O_RDONLY)) == -1)
    return ret;
  
  if(fstat(fd, &sb) == -1) 
    error("fstat failed");
  
  if(sb.st_size <= 0)
    goto cleanup_fd;
  
  if((buf = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
    error("mmap failed");
  
  if((search = memmem(buf, sb.st_size, semver_atom, strlen(semver_atom))) == NULL)
    goto cleanup;

  if((ret = malloc(max_semver_sz)) == NULL)
    error("malloc failed");
  
  strncpy(ret, search + offset_semver, max_semver_sz);

cleanup:
  munmap(buf, sb.st_size);
cleanup_fd:
  close(fd);
  return ret;
}

int main(int argc, char ** argv) {
  char * search;
  
  if(argc != 2) 
    error("invalid number of arguments!");
  
  if((search = search_semver(argv[1])) == NULL) {
    perror("search failed");
    error("semver not found");
  }
  
  printf("%s\n", search);
  free(search);
  return 0;
}
