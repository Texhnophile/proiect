#include"stdio.h"
#include"string.h"
#include"ctype.h"
#include"bmp_io_rgb.h"
#include"img_ops.h"

#define MAX_IMG 100
#define MAX_LEN 128

typedef struct
{
  char tag[MAX_LEN];
  unsigned char ***pix;
  int N, M;
} Image;

static Image imgs[MAX_IMG];
static int n_imgs = 0;

static Image *find_img(const char *tag)
{
  for(int i = 0; i < n_imgs; i++)
    if(strcmp(imgs[i].tag, tag) == 0)
      return &imgs[i];
  return NULL;
}

static void add_img(const char *tag, unsigned char ***pix, int N, int M)
{
  Image *im = find_img(tag);
  if(im)
    free_img(im->pix, im->N, im->M);
  else
    {
      im = &imgs[n_imgs++];
      strcpy(im->tag, tag);
    }
  im->pix = pix;
  im->N = N;
  im->M = M;
}

int main(int argc, char **argv)
{
  FILE *f = fopen(argv[1], "r");
  char cmd[32], tag[MAX_LEN], tag2[MAX_LEN], path[256];
  while(fscanf(f, "%31s", cmd) == 1)
    {
      for(int i = 0; cmd[i]; i++)
        cmd[i] = toupper((unsigned char)cmd[i]);
      printf("%s\n", cmd);
      if(strcmp(cmd, "CREATE") == 0)
        {
          int cols, rows;
          fscanf(f, "%s %d %d", tag, &cols, &rows);
          add_img(tag, alloc_img(rows, cols), rows, cols);
        }
      else if(strcmp(cmd, "LOAD") == 0)
        {
          int N, M;
          fscanf(f, "%s %s", tag, path);
          unsigned char ***pix = bmp_read(path, &N, &M);
          printf("LOADED: %s: N=%d M=%d\n", path, N, M);
          add_img(tag, pix, N, M);
        }
      else if(strcmp(cmd, "SAVE") == 0)
        {
          fscanf(f, "%s %s", tag, path);
          Image *im = find_img(tag);
          printf("SAVED %s: N=%d M=%d\n", path, im->N, im->M);
          bmp_write(path, im->pix, im->N, im->M);
        }
      else if(strcmp(cmd, "CLEAR") == 0)
        {
          int R, G, B;
          fscanf(f, "%s %d %d %d", tag, &R, &G, &B);
          Image *im = find_img(tag);
          printf("CLEARED %s: N=%d M=%d\n", path, im->N, im->M);
          clear_img(im->pix, im->N, im->M, R, G, B);
        }
      else if(strcmp(cmd, "PASTE") == 0)
        {
          int x, y;
          fscanf(f, "%s %s %d %d", tag, tag2, &x, &y);
          Image *d = find_img(tag), *s = find_img(tag2);
          paste_image(d->pix, d->N, d->M, s->pix, s->N, s->M, x, y);
        }
      else if(strcmp(cmd, "RECTANGLE") == 0)
        {
          int l, r, t, b, R, G, B;
          fscanf(f, "%s %d %d %d %d %d %d %d", tag, &l, &r, &t, &b, &R, &G, &B);
          Image *im = find_img(tag);
          draw_rectangle(im->pix, im->N, im->M, l, r, t, b, R, G, B);
        }
      else if(strcmp(cmd, "CROP") == 0)
        {
          int l, r, t, b, nN, nM;
          fscanf(f, "%s %d %d %d %d", tag, &l, &r, &t, &b);
          Image *im = find_img(tag);
          add_img(tag, crop(im->pix, im->N, im->M, l, r, t, b, &nN, &nM), nN, nM);
        }
      else if(strcmp(cmd, "GRAYSCALE") == 0)
        {
          fscanf(f, "%s", tag);
          Image *im = find_img(tag);
          grayscale(im->pix, im->N, im->M);
        }
      else if(strcmp(cmd, "SQUARE") == 0)
        {
          int side;
          fscanf(f, "%s", tag);
          Image *im = find_img(tag);
          add_img(tag, make_square(im->pix, im->N, im->M, &side), side, side);
        }
      else if(strcmp(cmd, "BOX") == 0)
        {
          fscanf(f, "%s", tag);
          Image *im = find_img(tag);
          box_blur(im->pix, im->N, im->M); 
        }
      else if(strcmp(cmd, "GAUSS") == 0)
        {
          fscanf(f, "%s", tag);
          Image *im = find_img(tag);
          gaussian_blur(im->pix, im->N, im->M); 
        }
      else if(strcmp(cmd, "SHARPEN") == 0)
        {
          fscanf(f, "%s", tag);
          Image *im = find_img(tag);
          sharpen(im->pix, im->N, im->M); 
        }
      else if(strcmp(cmd, "LAPLACE") == 0)
        {
          fscanf(f, "%s", tag);
          Image *im = find_img(tag);
          laplacian(im->pix, im->N, im->M); 
        }
      else if(strcmp(cmd, "EMBOSS") == 0)
        {
          fscanf(f, "%s", tag);
          Image *im = find_img(tag);
          emboss(im->pix, im->N, im->M); 
        }
    }
  fclose(f);
  for(int i = 0; i < n_imgs; i++)
    free_img(imgs[i].pix, imgs[i].N, imgs[i].M);
  return 0;
}

