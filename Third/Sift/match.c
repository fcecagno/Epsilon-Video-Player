/*
Detects SIFT features in two images and finds matches between them.

Copyright (C) 2006-2010  Rob Hess <hess@eecs.oregonstate.edu>

@version 1.1.2-20100521
*/

#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"
#include "xform.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <stdio.h>


/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49

/******************************** Globals ************************************/

/********************************** Main *************************************/

int main( int argc, char** argv )
{
	IplImage *img1, * img2, *img_chk, * stacked, * aligned, *aligned2;
	struct feature* feat1, * feat2, * feat;
	struct feature** nbrs;
	struct kd_node* kd_root;
	CvPoint pt1, pt2;
	double d0, d1;
	int n1, n2, k, i, m = 0;
	FILE *out = fopen(argv[3], "w");
	int f1x[1000], f1y[1000];
	int f2x[1000], f2y[1000];
	unsigned char r, g, b;

	char *img1_file = argv[1];
	char *img2_file = argv[2];
	char *imgchk_file = argv[4];

	img1 = cvLoadImage( img1_file, 1 );
	if( ! img1 )
		fatal_error( "unable to load image from %s", img1_file );
	img2 = cvLoadImage( img2_file, 1 );
	if( ! img2 )
		fatal_error( "unable to load image from %s", img2_file );
	stacked = stack_imgs( img1, img2 );
	aligned = align_imgs( img1, img2 );
	aligned2 = align_imgs( img1, img2 );

	img_chk = cvLoadImage( imgchk_file, 1 );

	fprintf( stderr, "Finding features in %s...\n", img1_file );
	n1 = sift_features( img1, &feat1 );
	fprintf( stderr, "Finding features in %s...\n", img2_file );
	n2 = sift_features( img2, &feat2 );
	kd_root = kdtree_build( feat2, n2 );

	for( i = 0; i < n1; i++ )
	{
		feat = feat1 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 )
		{
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );

				f1x[m] = pt1.x;
				f1y[m] = pt1.y;
				f2x[m] = pt2.x;
				f2y[m] = pt2.y;

				//if(i < 100) {
				//pt1.x = i;
				//pt1.y = i;

				//r = img_chk->imageData[(pt1.y*(img_chk->width) + pt1.x)*3];
				//g = img_chk->imageData[(pt1.y*(img_chk->width) + pt1.x)*3+1];
				//b = img_chk->imageData[(pt1.y*(img_chk->width) + pt1.x)*3+2];
				//if((r == 0)&&
				//   (g == 255)&&
				//   (b == 0))
				//   continue;

				cvLine(stacked, pt1, cvPoint(pt2.x, pt2.y+img1->height), CV_RGB(255,0,255), 1, 8, 0);
				cvLine(aligned, pt1, cvPoint(pt2.x+img1->width, pt2.y), CV_RGB(255,0,255), 1, 8, 0);
				cvCircle(aligned2, pt1, 2, CV_RGB(255,0,255), 1, 8, 0);
				cvCircle(aligned2, cvPoint(pt2.x+img1->width, pt2.y), 2, CV_RGB(255,0,255), 1, 8, 0);
				//}

				m++;
				feat1[i].fwd_match = nbrs[0];
			}
		}
		free( nbrs );
	}

	fprintf( stderr, "Found %d total matches\n", m );
	cvSaveImage(argv[4], stacked, NULL);
	cvSaveImage(argv[5], aligned, NULL);
	cvSaveImage(argv[6], aligned2, NULL);
	//cvNamedWindow( "Matches", 1 );
	//cvShowImage( "Matches", stacked );
	//cvWaitKey( 0 );


	fprintf(out, "%d\n", m);
	for(i = 0; i < m; ++i)
		fprintf(out, "%d\n", img1->height-f1y[i]);
	for(i = 0; i < m; ++i)
		fprintf(out, "%d\n", f1x[i]);
	for(i = 0; i < m; ++i)
		fprintf(out, "%d\n", img1->height-f2y[i]);
	for(i = 0; i < m; ++i)
		fprintf(out, "%d\n", f2x[i]);

	/* 
	UNCOMMENT BELOW TO SEE HOW RANSAC FUNCTION WORKS

	Note that this line above:

	feat1[i].fwd_match = nbrs[0];

	is important for the RANSAC function to work.
	*/
	/*
	{
		CvMat* H;
		H = ransac_xform( feat1, n1, FEATURE_FWD_MATCH, lsq_homog, 4, 0.01,
			homog_xfer_err, 3.0, NULL, NULL );
		if( H )
		{
			IplImage* xformed;
			xformed = cvCreateImage( cvGetSize( img2 ), IPL_DEPTH_8U, 3 );
			cvWarpPerspective( img1, xformed, H, 
				CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS,
				cvScalarAll( 0 ) );
			cvNamedWindow( "Xformed", 1 );
			cvShowImage( "Xformed", xformed );
			cvWaitKey( 0 );
			cvReleaseImage( &xformed );
			cvReleaseMat( &H );
		}
	}
	*/

	cvReleaseImage( &stacked );
	cvReleaseImage( &img1 );
	cvReleaseImage( &img2 );
	kdtree_release( kd_root );
	free( feat1 );
	free( feat2 );
	return 0;
}
