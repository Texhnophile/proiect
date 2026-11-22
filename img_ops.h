#ifndef IMG_OPS_H
#define IMG_OPS_H
#include <stdlib.h>

static inline void free_img(unsigned char ***img, int N, int M)
{
  for(int i = 0; i < N; i++)
    {
      if(img[i])
        {
          for(int j = 0; j < M; j++)
            free(img[i][j]);
        }
      free(img[i]);
    }
  free(img);
}

static inline unsigned char ***alloc_img(int N, int M)
{
  unsigned char ***img = calloc(N, sizeof(*img));
  for(int i = 0; i < N; i++)
    {
      img[i]=calloc(M, sizeof((**img)));
      for(int j = 0; j < M; j++)
        {
          img[i][j] = calloc(3, 1);
        }
    }
  return img;
}

static inline void grayscale(unsigned char ***img, int N, int M)
{
  for (int i = 0; i < N; i++)
    for (int j = 0; j < M; j++) {
      unsigned char gray = (img[i][j][0] + img[i][j][1] + img[i][j][2]) / 3;
      img[i][j][0] = gray;
      img[i][j][1] = gray;
      img[i][j][2] = gray;
    }
}
static inline unsigned char ***make_square(unsigned char ***img, int N, int M, int *side)
{
  int s = (N > M) ? N : M;
  unsigned char ***out = alloc_img(s, s);
  if (!out) return NULL;
  int offR = (s - N) / 2;  int offC = (s - M) / 2;
  for (int i = 0; i < s; i++)
    for (int j = 0; j < s; j++)
      for (int k = 0; k < 3; k++)
        out[i][j][k] = 255;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < M; j++)
      for (int k = 0; k < 3; k++)
        out[offR + i][offC + j][k] = img[i][j][k];
  *side = s;
  return out;
}
static inline unsigned char ***crop(unsigned char ***img, int N, int M, int left, int right, int top, int bottom, int *newN, int *newM)
{
  int h = bottom-top+1; int w=right-left+1;
  unsigned char ***out = alloc_img(h, w);
  for(int i = 0; i < h; i++)
    for(int j = 0; j < w; j++)
      for(int k = 0; k < 3; k++)
        out[i][j][k] = img[top+i][left+j][k];
  *newN=h;
  *newM=w;
  return out;
}
static inline int paste_image(unsigned char ***base_img, int N_base, int M_base, unsigned char ***pasted_image, int N_paste, int M_paste, int x, int y)
{
  for(int i = 0; i < N_paste; i++)
    for(int j = 0; j < M_paste; j++)
      {
        int r = y + i; int c = x + j;
        if(r < 0 || r >= N_base || c < 0 || c >= M_base) continue;
        for(int k = 0; k < 3; k++)
          base_img[r][c][k] = pasted_image[i][j][k];
      }
  return 1;
}

static inline void set_pixel(unsigned char ***img, int N, int M, int R, int G, int B, int r, int c)
{
  img[r][c][0] = R;
  img[r][c][1] = G;
  img[r][c][2] = B;
}

static inline void draw_rectangle(unsigned char ***img, int N, int M, int left, int right, int up, int down, int R, int G, int B)
{
  for(int j = left; j < right; j++)
    {
      set_pixel(img, N, M, R, G, B, up, j);
      set_pixel(img, N, M, R, G, B, down, j);
    }
  for(int i = up; up < down; i++)
    {
      set_pixel(img, N, M, R, G, B, i, left);
      set_pixel(img, N, M, R, G, B, i, right);
    }
}
static inline void clear_img(unsigned char ***img, int N, int M, int R, int G, int B)
{
  for(int i = 0; i < N; i++)
    for(int j = 0; j < M; j++)
      set_pixel(img, N, M, R, G, B, i, j);
}

static inline void convolve3x3(unsigned char ***img, int N, int M, const int kernel[3][3], int divisor)
{
  unsigned char ***src = alloc_img(N, M);
  for(int i = 0; i < N; i++)
    for(int j = 0; j < M; j++)
      for(int k = 0; k < 3; k++)
        src[i][j][k] = img[i][j][k];
  for(int i = 0; i < N; i++)
    for(int j = 0; j < M; j++)
      for(int k = 0; k < 3; k++)
        {
          int sum = 0;
          for(int di = -1; di <= 1; di++)
            for(int dj = -1; dj <= 1; dj++)
              {
                int r = i + di, c = j + dj;
                if(r < 0) r = 0;
                if(r >= N) r = N - 1;
                if(c < 0) c = 0;
                if(c >= M) c = M - 1;
                sum += kernel[di + 1][dj + 1] * src[r][c][k];
              }
          img[i][j][k] = sum / divisor;
        }
  free_img(src, N, M);
}

static inline void box_blur(unsigned char ***img, int N, int M)
{
  const int kernel[3][3] = {{1, 1, 1},
                            {1, 1, 1},
                            {1, 1, 1}};
  convolve3x3(img, N, M, kernel, 9);
}

static inline void gaussian_blur(unsigned char ***img, int N, int M)
{
  const int kernel[3][3] = {{1, 2, 1},
                            {2, 4, 2},
                            {1, 2, 1}};
  convolve3x3(img, N, M, kernel, 16);
}

static inline void sharpen(unsigned char ***img, int N, int M)
{
  const int kernel[3][3] = {{ 0, -1,  0},
                            {-1,  5, -1},
                            { 0, -1,  0}};
  convolve3x3(img, N, M, kernel, 1);
}

static inline void laplacian(unsigned char ***img, int N, int M)
{
  const int kernel[3][3] = {{-1, -1, -1},
                            {-1,  8, -1},
                            {-1, -1, -1}};
  convolve3x3(img, N, M, kernel, 1);
}

static inline void emboss(unsigned char ***img, int N, int M)
{
  const int kernel[3][3] = {{-2, -1, 0},
                            {-1,  1, 1},
                            { 0,  1, 2}};
  convolve3x3(img, N, M, kernel, 1);
}
#endif

