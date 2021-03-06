#ifndef KUDU_UTIL_ALIGNMENT_H
#define KUDU_UTIL_ALIGNMENT_H

// Round down 'x' to the nearest 'align' boundary
#define KUDU_ALIGN_DOWN(x, align) ((x) & (-(align)))

// Round up 'x' to the nearest 'align' boundary
#define KUDU_ALIGN_UP(x, align) (((x) + ((align) - 1)) & (-(align)))

#endif
