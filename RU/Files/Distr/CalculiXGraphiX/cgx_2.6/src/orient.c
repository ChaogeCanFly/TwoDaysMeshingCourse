/* --------------------------------------------------------------------  */
/*                          CALCULIX                                     */
/*                   - GRAPHICAL INTERFACE -                             */
/*                                                                       */
/*     A 3-dimensional pre- and post-processor for finite elements       */
/*              Copyright (C) 1996 Klaus Wittig                          */
/*                                                                       */
/*     This program is free software; you can redistribute it and/or     */
/*     modify it under the terms of the GNU General Public License as    */
/*     published by the Free Software Foundation; version 2 of           */
/*     the License.                                                      */
/*                                                                       */
/*     This program is distributed in the hope that it will be useful,   */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of    */ 
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the      */
/*     GNU General Public License for more details.                      */
/*                                                                       */
/*     You should have received a copy of the GNU General Public License */
/*     along with this program; if not, write to the Free Software       */
/*     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.         */
/* --------------------------------------------------------------------  */

#include <cgx.h>

#define TEST 0

extern Points    *point;
extern Lines     *line;
extern Lcmb      *lcmb;
extern Gsur      *surf;
extern Gbod      *body;
extern Nurbs     *nurbs;
extern SumGeo    anzGeo[1];


/* Set Management */
extern Sets      *set;
extern Scale     scale[1];
extern SpecialSet specialset[1];
extern char  printFlag;                     /* printf on/off */



double angleSurfBody( double *v0, double *v1, double *v2, double *scg, double *bcg )
{
  double vsb[3], vsb_[3];
  double v01[3], v02[3], vn[3], vn_[3];
  double fi;

   /* vektor surf-body */
    v_result( scg, bcg, vsb );
    v_norm(  vsb, vsb_ ); 
    /* normale auf surf */
    v_result( v0, v1, v01 );
    v_result( v0, v2, v02 );
    v_prod( v01, v02, vn );
    v_norm( vn, vn_ ); 
    /* winkel zwischen den Vektoren */
    fi=v_sprod( vsb_, vn_ );
    return(fi);
}


/* lcmb rueckgabe von 1. linie bis n.linie orientiert (linie*ori zeigt in richtung letzte linie) */
int orientLcmb( int nr )
{
  register int i,j;
  int     flag1, flag2, pnt=0, err=0, lcount=0;
  static int     *sortLines=NULL;
  static char    *sortOri=NULL;

  if(printFlag) printf (" orient lcmb:%s ", lcmb[nr].name);

  /* loesche alle linien orientierungen */
  for (i=0; i<lcmb[nr].nl; i++)
  {
    lcmb[nr].o[i]=0;
  }

  if((sortOri = (char *)realloc((char *)sortOri, (lcmb[nr].nl)*sizeof(char)) ) == NULL )
  { printf(" ERROR: realloc failure in orientLcmb()\n\n"); return(-1); }
  if((sortLines=(int *)realloc((int *)sortLines, (lcmb[nr].nl)*sizeof(int) ) )==NULL)
  { printf(" ERROR: realloc failure in orientLcmb()\n\n"); return(-1); }

  /* suche die erste linie (suche einen punkt der nur mit einer linie verbunden ist) */
  /* und orientiere diese */
  for (i=0; i<lcmb[nr].nl; i++)
  {
    flag1=flag2=0;
    for (j=0; j<lcmb[nr].nl; j++)
    {
      if (i!=j)
      {
        if ( line[lcmb[nr].l[i]].p1 == line[lcmb[nr].l[j]].p1 ) flag1=1;
        if ( line[lcmb[nr].l[i]].p1 == line[lcmb[nr].l[j]].p2 ) flag1=1;
        if ( line[lcmb[nr].l[i]].p2 == line[lcmb[nr].l[j]].p1 ) flag2=1;
        if ( line[lcmb[nr].l[i]].p2 == line[lcmb[nr].l[j]].p2 ) flag2=1;
      }
    }
    if (!flag1)
    {
      /* p1 ist frei, linie ist + orientiert */
      lcmb[nr].o[i]='+';
      pnt=line[lcmb[nr].l[i]].p2;
      break;
    }
    if (!flag2)
    {
      /* p2 ist frei, linie ist - orientiert */
      lcmb[nr].o[i]='-';
      pnt=line[lcmb[nr].l[i]].p1;
      break;
    }
  }
  if (i==lcmb[nr].nl)
  {
    errMsg(" ERROR: lcmb %s are a closed loop, no orientation possible\n", lcmb[nr].name);
    for (j=0; j<lcmb[nr].nl; j++) printf(" %s", line[lcmb[nr].l[j]].name);
    printf("\n");
    err=-i;
  }
  else
  {
    sortLines[lcount]=lcmb[nr].l[i];      /* merke dir die reihenfolge in der die linien kommen */
    sortOri[lcount]=  lcmb[nr].o[i];      /* merke dir die dazugehoerigen orientationen */
    lcount++;
  }

  for (i=1; i<lcmb[nr].nl; i++)  /* ori die restlichen linien */
  {
    for (j=0; j<lcmb[nr].nl; j++) /* suche die anschliessende durch vergleich */
    {
      if (!lcmb[nr].o[j])              /* wenn ori noch == 0 */
      {
        /*
        printf ("pnt:%s \n", point[pnt].name);
        */
        if ( line[lcmb[nr].l[j]].p1 == pnt )
        {  pnt=line[lcmb[nr].l[j]].p2; lcmb[nr].o[j]='+';
           sortLines[lcount]=lcmb[nr].l[j]; sortOri[lcount]=lcmb[nr].o[j]; lcount++; break; }
        if ( line[lcmb[nr].l[j]].p2 == pnt )
        {  pnt=line[lcmb[nr].l[j]].p1; lcmb[nr].o[j]='-';
           sortLines[lcount]=lcmb[nr].l[j]; sortOri[lcount]=lcmb[nr].o[j]; lcount++; break; }
      }
    }
  }

  /* check for unoriented lines */
  for (i=0; i<lcmb[nr].nl; i++)
  {
    if (!lcmb[nr].o[i])
    {
      err--;
      errMsg(" ERROR: l:%s in lcmb:%s  not chained and therefore not oriented.\n", line[lcmb[nr].l[i]].name, lcmb[nr].name );
      /* unverbundene linien werden am ende angehaengt */
      sortLines[lcount]=lcmb[nr].l[i];  sortOri[lcount]='+'; lcount++;
    }
  }
  if (lcount!=lcmb[nr].nl) errMsg(" ERROR: lcount:%d != %d lines in lcmb:%s\n",
     lcount,  lcmb[nr].nl, lcmb[nr].name );

  /* rearange lines */
  for (i=0; i<lcmb[nr].nl; i++)
  {
    lcmb[nr].o[i]=sortOri[i];
    lcmb[nr].l[i]=sortLines[i];
    if(printFlag) printf ("%1c %s ", lcmb[nr].o[i], line[lcmb[nr].l[i]].name );
  }
  if(printFlag) printf ("\n");

  /* determine the end-points */
  if (lcmb[nr].o[0]=='+')           lcmb[nr].p1=line[lcmb[nr].l[0]].p1;  
  else                              lcmb[nr].p1=line[lcmb[nr].l[0]].p2;
  if (lcmb[nr].o[lcmb[nr].nl-1]=='+') lcmb[nr].p2=line[lcmb[nr].l[lcmb[nr].nl-1]].p2;
  else                              lcmb[nr].p2=line[lcmb[nr].l[lcmb[nr].nl-1]].p1;

  /*
  printf("l0:%s p1:%s p2:%s   l2:%s p1:%s p2:%s \n", line[lcmb[nr].l[0]].name,
    point[line[lcmb[nr].l[0]].p1].name, point[line[lcmb[nr].l[0]].p2].name,
    line[lcmb[nr].l[lcmb[nr].nl-1]].name, point[line[lcmb[nr].l[lcmb[nr].nl-1]].p1].name,
    point[line[lcmb[nr].l[lcmb[nr].nl-1]].p2].name);
  printf ("p1:%s p2:%s\n", point[lcmb[nr].p1].name, point[lcmb[nr].p2].name ); 
  */

  return(err);
}


/* surf rueckgabe von 1. linie bis n.linie orientiert. linie*ori zeigt in richtung letzte linie */
/* der aussenloop ist mathematisch positiv, die inneren negativ (wie von libGLU benoetigt) */
int orientSurf( int nr )
{
  int i,j=0,k, l, n=0;
  int     flag, flag1, flag2, err=0, lcount=0;
  int     *sortLines=NULL;
  int     p1,p2, pnt=0, sl, cl, l0;
  int     line0_p1, line0_p2;
  static char    *sortOri=NULL, *sortTyp=NULL;
  int *cp=NULL;

  int *np, c_outer, p, nip;
  double **xyz, lmax, v0[3], v1[3], v2[3], v0v1[3], v1v2[3],vn[3],vn_ges[100][3],lloop[100];

  if(printFlag) printf (" orient surf:%s ", surf[nr].name);

  /* loesche alle linien orientierungen */
  for (i=0; i<surf[nr].nl; i++)
  {
    surf[nr].o[i]=0;
  }
  surf[nr].nc= 0;

  if((sortOri = (char *)realloc((char *)sortOri, (surf[nr].nl)*sizeof(char)) ) == NULL )
  { printf(" ERROR: realloc failure in orientSurf()\n\n"); return(-1); }
  if((sortTyp = (char *)realloc((char *)sortTyp, (surf[nr].nl)*sizeof(char)) ) == NULL )
  { printf(" ERROR: realloc failure in orientSurf()\n\n"); return(-1); }
  if((sortLines=(int *)realloc((int *)sortLines, (surf[nr].nl)*sizeof(int) ) )==NULL)
  { printf(" ERROR: realloc failure in orientSurf()\n\n"); return(-1); }
  if((cp=(int *)realloc((int *)cp, (surf[nr].nl)*sizeof(int) ) )==NULL)
  { printf(" ERROR: realloc failure in orientSurf()\n\n"); return(-1); }


  /* Definiere die Linienzuege (curves) zb: Auessere Umrandung, innere Loecher */
  /* die einzelnen Linenzuege werden zum NURBS-Trimming benutzt. Zum Vernetzen sind */
  /* nur Surfaces mit einem Linienzug geeignet */
  do
  {
#if TEST    
    printf("curve starts with n:%d lcount:%d \n",n,lcount);
#endif
    /* suche den punkt einer linie die mit der ersten linie verbunden ist */
    /* und orientiere die erste linie */
    if (surf[nr].typ[n]=='c')
    {
      sl=surf[nr].l[n];
      cl=lcmb[sl].nl-1;
        /* erste linie der lcmb enthaelt den 1.p fuer den vergl. */
        /* letzte linie der lcmb enthaelt den 2.p fuer den vergl. */
        if(lcmb[sl].o[0]=='+')  line0_p1=line[lcmb[sl].l[0]].p1;
        else                    line0_p1=line[lcmb[sl].l[0]].p2;
        if(lcmb[sl].o[cl]=='+') line0_p2=line[lcmb[sl].l[cl]].p2;
        else                    line0_p2=line[lcmb[sl].l[cl]].p1;
    }
    else
    {
      l0=surf[nr].l[n];
      line0_p1=line[l0].p1;
      line0_p2=line[l0].p2;
    }
  
#if TEST    
    for (i=0; i<surf[nr].nl; i++)
    {
      if (surf[nr].typ[i]=='c')
        printf("%1c %1c %s\n",surf[nr].typ[i], surf[nr].o[i], lcmb[surf[nr].l[i]].name);
      else if (surf[nr].typ[i]=='l')
        printf("%1c %1c %s\n", surf[nr].typ[i], surf[nr].o[i], line[surf[nr].l[i]].name);
      else printf("unknown line\n");
    }
#endif
  
    /* lcmb muessen sortiert vorliegen */
    /* gehe durch alle Linien und suche die zweite Linie */
    flag1=flag2=0;
    for (i=n+1; i<surf[nr].nl; i++)
    {
      if (surf[nr].typ[i]=='c')
      {
        sl=surf[nr].l[i];
        cl=lcmb[sl].nl-1;
        /* erste linie der lcmb enthaelt den 1.p fuer den vergl. */
        /* letzte linie der lcmb enthaelt den 2.p fuer den vergl. */
        if(lcmb[sl].o[0]=='+')  p1=line[lcmb[sl].l[0]].p1;
        else                    p1=line[lcmb[sl].l[0]].p2;
        if(lcmb[sl].o[cl]=='+') p2=line[lcmb[sl].l[cl]].p2;
        else                    p2=line[lcmb[sl].l[cl]].p1;
#if TEST    
    printf ("%1c lc1:%s %1c lc2:%s \n", lcmb[sl].o[0], line[lcmb[sl].l[0]].name,
                                        lcmb[sl].o[cl], line[lcmb[sl].l[cl]].name);
    printf (" p1:%s p2:%s \n", point[p1].name, point[p2].name);
#endif
      }
      else
      {
        p1=line[surf[nr].l[i]].p1;
        p2=line[surf[nr].l[i]].p2;
#if TEST    
    printf ("l:%s  \n", line[surf[nr].l[i]].name );
#endif
      }
  
      if      ( line0_p1 == p1 ) flag1=1;
      else if ( line0_p2 == p1 ) flag2=1;
      else if ( line0_p1 == p2 ) flag1=2;
      else if ( line0_p2 == p2 ) flag2=2;
      if (flag1==1)
      {
          surf[nr].o[n]='-';
          surf[nr].o[i]='+';
          pnt=p2;
          break;
      }
      else if (flag1==2)
      {
          surf[nr].o[n]='-';
          surf[nr].o[i]='-';
          pnt=p1;
          break;
      }
      else if (flag2==1)
      {
          surf[nr].o[n]='+';
          surf[nr].o[i]='+';
          pnt=p2;
          break;
      }
      else if (flag2==2)
      {
          surf[nr].o[n]='+';
          surf[nr].o[i]='-';
          pnt=p1;
          break;
      }
      else pnt=0;
    }

    if (i==surf[nr].nl)
    {
      errMsg("WARNING: surf not closed, no orientation possible\n");
      for (i=0; i<surf[nr].nl; i++)
      {
        surf[nr].o[i]='+';
      }
      return(-1);
    }
    else
    {
      sortLines[lcount]=surf[nr].l[n];      /* merke dir die reihenfolge in der die linien kommen */
      sortOri[lcount]=  surf[nr].o[n];      /* merke dir die dazugehoerigen orientationen */
      sortTyp[lcount]=  surf[nr].typ[n];
      lcount++;
      sortLines[lcount]=surf[nr].l[i];      /* merke dir die reihenfolge in der die linien kommen */
      sortOri[lcount]=  surf[nr].o[i];      /* merke dir die dazugehoerigen orientationen */
      sortTyp[lcount]=  surf[nr].typ[i];
      lcount++;
    }

#if TEST    
   printf ("Curve:%d Start-line:%s ori:%c 2line:%s ori:%c \n",surf[nr].nc,line[surf[nr].l[n]].name, surf[nr].o[n], line[surf[nr].l[i]].name, surf[nr].o[i] );
#endif
    for (i=n+1; i<surf[nr].nl; i++)  /* ori die restlichen linien */
    {
      for (j=0; j<surf[nr].nl; j++) /* suche die anschliessende durch vergleich */
      {
        if (!surf[nr].o[j])              /* wenn ori noch == 0 */
        {
#if TEST    
          printf ("pnt:%s nr:%d j:%d\n", point[pnt].name, nr,j);
#endif
          if (surf[nr].typ[j]=='c')
          {
            sl=surf[nr].l[j];
            cl=lcmb[sl].nl-1;
            if(lcmb[sl].o[0]=='+')     p1=line[lcmb[sl].l[0]].p1;
            else                       p1=line[lcmb[sl].l[0]].p2;
            if(lcmb[sl].o[cl]=='+')   p2=line[lcmb[sl].l[cl]].p2;
            else                      p2=line[lcmb[sl].l[cl]].p1;
#if TEST    
   printf ("lcmb:%s pnt:%d p1:%d p2:%d\n", lcmb[surf[nr].l[j]].name, pnt, p1, p2 );
#endif
          }
          else
          {
            p1=line[surf[nr].l[j]].p1;
            p2=line[surf[nr].l[j]].p2;
#if TEST    
   printf ("i:%d j:%d line:%s ori:%c pnt:%d p1:%d p2:%d\n", i,j,line[surf[nr].l[j]].name, surf[nr].o[j], pnt, p1, p2 );
#endif
          }
  
          if ( p1 == pnt )
          {  pnt=p2; surf[nr].o[j]='+';
#if TEST    
   printf ("i:%d j:%d line:%s pnt:%d p1:%d p2:%d\n", i,j,line[surf[nr].l[j]].name, pnt, p1, p2 );
#endif
             sortLines[lcount]=surf[nr].l[j]; sortOri[lcount]=surf[nr].o[j];
             sortTyp[lcount]=surf[nr].typ[j]; lcount++; }
          else if ( p2 ==  pnt )
          {  pnt=p1; surf[nr].o[j]='-';
#if TEST    
   printf ("i:%d j:%d line:%s pnt:%d p1:%d p2:%d\n", i,j,line[surf[nr].l[j]].name, pnt, p1, p2 );
#endif
             sortLines[lcount]=surf[nr].l[j]; sortOri[lcount]=surf[nr].o[j];
             sortTyp[lcount]=surf[nr].typ[j]; lcount++; }
        }
      }
    }

    /* check if the curve is closed and store the first line-index of the surf */
    if(sortTyp[n]=='l')
    {
      if((pnt==line[sortLines[n]].p1)||(pnt==line[sortLines[n]].p2))
      {
        /* start-index of the curve */
        if((surf[nr].c=(int *)realloc((int *)surf[nr].c, (surf[nr].nc+1)*sizeof(int)) )==NULL)
        { printf(" ERROR: realloc failure in orient surf:%s can not be oriented\n", surf[nr].name); exit(-1); }
        surf[nr].c[surf[nr].nc]=lcount-n;
        surf[nr].nc++;
      }
    }
    else if(sortTyp[n]=='c')
    {
      if((pnt==lcmb[sortLines[n]].p1)||(pnt==lcmb[sortLines[n]].p2))
      {
        /* start-index of the curve */
        if((surf[nr].c=(int *)realloc((int *)surf[nr].c, (surf[nr].nc+1)*sizeof(int)) )==NULL)
        { printf(" ERROR: realloc failure in orient surf:%s can not be oriented\n", surf[nr].name); exit(-1); }
        surf[nr].c[surf[nr].nc]=lcount-n;
        surf[nr].nc++;
      }
    }
    else
    {
      printf("WARNING in ori: Surf:%s has an unclosed curve\n", surf[nr].name);
      printf("typ:%c pnt:%s line[sortLines[n]].p1:%s line[sortLines[n]].p2:%s\n", sortTyp[n], point[pnt].name, point[line[sortLines[n]].p1].name, point[line[sortLines[n]].p2].name);
      goto finish_ori;
    }

    /* check for unoriented lines */
    err=0;
    for (n=0; n<surf[nr].nl; n++)
    {
      if (!surf[nr].o[n])
      {
        err--;
        break;
      }
    }
  }while(err<0);


  finish_ori:;
  /* check of the loop was closed */
  if(surf[nr].nc==0)
  {
    errMsg(" ERROR: surf:%s not chained and therefore not oriented.\n", surf[nr].name );
    err--;
  }

  /* check for unoriented lines */
  for (i=0; i<surf[nr].nl; i++)
  {
    if (!surf[nr].o[i])
    {
      err--;
      if (surf[nr].typ[i]=='l')
        errMsg(" ERROR: l:%s in surf:%s not chained and therefore not oriented.\n", line[surf[nr].l[i]].name, surf[nr].name );
      else
        errMsg(" ERROR: c:%s in surf:%s not chained and therefore not oriented.\n", lcmb[surf[nr].l[i]].name, surf[nr].name );
      /* unverbundene linien werden am ende  angehaengt */
      sortLines[lcount]=surf[nr].l[i];  sortOri[lcount]='+'; sortTyp[lcount]=surf[nr].typ[i]; lcount++;
    }
  }
  if (lcount!=surf[nr].nl) errMsg(" ERROR: lcount:%d != %d lines in surf:%s\n",
     lcount,  surf[nr].nl, surf[nr].name );

  /* rearange lines */
  for (i=0; i<surf[nr].nl; i++)
  {
    surf[nr].typ[i]=sortTyp[i];
    surf[nr].o[i]=sortOri[i];
    surf[nr].l[i]=sortLines[i];
    if ((surf[nr].typ[i]=='l')&&(printFlag)) printf("%1c %s ", surf[nr].o[i], line[surf[nr].l[i]].name );
    else if(printFlag) printf ("%1c %s ", surf[nr].o[i], lcmb[surf[nr].l[i]].name );
  }
  if(printFlag) printf ("\n");

  /* orientiere die einzelnen loops */
  /* der aeussere ist mathematisch positiv und der rest negativ */
  /* fuer jeden loop wird das kreuzprodukt ueber alle linien gebildet */
  goto jump_over_arrange_curves;

  if( (np= (int *)malloc( (surf[nr].nc+1)*sizeof(int) )) == NULL )
  { printf(" ERROR: realloc failure\n\n"); return(-1); }
  if( (xyz = (double **)malloc( (surf[nr].nc+1)*sizeof(double *) )) == NULL )
  { printf(" ERROR: realloc failure\n\n"); return(-1); }

  k=0;
  for (i=0; i<surf[nr].nc; i++)
  {
    p=0;
    nip=0;
    for(j=0; j<surf[nr].c[i]; j++)
    {
      /* store the locations of all line-points, they will be projected to the NURBS */
      if(surf[nr].typ[k]=='l')
      {
        l=surf[nr].l[k];
        nip+=line[l].nip;

        if( (xyz[i] = (double *)realloc( (double *)xyz[i], (nip)*sizeof(double ) ))==NULL )
        { printf(" ERROR: realloc failure\n\n"); return(-1); }

        if(surf[nr].o[k]=='+')
        {             
          if(j==0)                     { n=0; flag=line[l].nip-3; }
          else if(j==surf[nr].c[i]-1)    { n=0; flag=line[l].nip; }
          else                         { n=0; flag=line[l].nip-3; }
          do
          {
            xyz[i][p]=line[l].ip[n++];
            p++;
          }while(n<flag);
        }
        else
        {             
          if(j==0)                     { n=line[l].nip; flag=3; }
          else if(j==surf[nr].c[i]-1)    { n=line[l].nip; flag=0; }
          else                         { n=line[l].nip; flag=3; }
          while(n>flag)
          {
            xyz[i][p+2]=line[l].ip[--n];
            xyz[i][p+1]=line[l].ip[--n];
            xyz[i][p]=line[l].ip[--n];
            p+=3;
          }
        }
      }
      else
      {
        for(cl=0; cl<lcmb[surf[nr].l[k]].nl; cl++)
        {
          l=lcmb[surf[nr].l[k]].l[cl];

          nip+=line[l].nip;
          if( (xyz[i] = (double *)realloc( (double *)xyz[i], (nip)*sizeof(double ) ))==NULL )
          { printf(" ERROR: realloc failure\n\n"); return(-1); }
          if(surf[nr].o[k]=='+') flag=1; else flag=-1;
          if(lcmb[surf[nr].l[k]].o[cl]=='+') flag*=1; else flag*=-1; 

          if(flag==1)
          {
            if(j==0)                     { n=0; flag=line[l].nip-3; }
            else if(j==surf[nr].c[i]-1)    { n=0; flag=line[l].nip; }
            else                         { n=0; flag=line[l].nip-3; }
            do
            {
              xyz[i][p]=line[l].ip[n++];
              p++;
            }while(n<flag);
          }
          else
          {
            if(j==0)                     { n=line[l].nip; flag=3; }
            else if(j==surf[nr].c[i]-1)    { n=line[l].nip; flag=0; }
            else                         { n=line[l].nip; flag=3; }
            while(n>flag)
            {
              xyz[i][p+2]=line[l].ip[--n];
              xyz[i][p+1]=line[l].ip[--n];
              xyz[i][p]=line[l].ip[--n];
              p+=3;
            }
          }

        }
      }

      k++;
    }
    np[i]=p;
  }
  
  /* calculate the cross-product for all loops */
  for (i=0; i<surf[nr].nc; i++)
  {
    p=0;lmax=0.;
    do
    {
        /* add all normals */
      v0[0]=xyz[i][p];
      v0[1]=xyz[i][p+1];
      v0[2]=xyz[i][p+2];
      v1[0]=xyz[i][p+3];
      v1[1]=xyz[i][p+4];
      v1[2]=xyz[i][p+5];
      v2[0]=xyz[i][p+6];
      v2[1]=xyz[i][p+7];
      v2[2]=xyz[i][p+8];
      v_result(v0, v1, v0v1);
      v_result(v1, v2, v1v2);
      v_prod(v0v1,v1v2,vn);
      v_add(vn_ges[i], vn, vn_ges[i] );
      p+=3;
    }while(p<np[i]-8);

    /* determine the outer loop based on the longest vector of the cross-prod */  
    lloop[i]=v_betrag(vn_ges[i]);
    if(lloop[i]>lmax) { lmax=lloop[i]; c_outer=i; }
  }

  /* rearrange if the orientation of the inner loops are in the same directin than the outer */
  for (i=0; i<surf[nr].nc; i++)
  {
    /* determine the angle between the normals of the outer and inner curves */
    /* if the angle is >90deg, rearrange the inner curve */
  }

  /* if the outer loop is not the first, rearrange */

  free(np);
  for(i=0; i<surf[nr].nc; i++) free(xyz[i]); free(xyz);
 jump_over_arrange_curves:;


  /* suche die Eckknoten der surface */
  /* beruecksichtige dabei die orientierung der linien oder lcmbs */
  /* so das die punktefolge der linienfolge entspricht */
  /* aus den eckknoten wird der Schwerpunkt (CG) bestimmt */

  /* suche den ersten punkt der ersten linie oder lcmb der surf */
  sl=surf[nr].l[0];
  if (surf[nr].typ[0]=='c')
  {
    cl=lcmb[sl].nl-1;
    if (surf[nr].o[0]=='-')
    {
      if(lcmb[sl].o[cl]=='+') cp[0]=line[lcmb[sl].l[cl]].p2;
      else                    cp[0]=line[lcmb[sl].l[cl]].p1;
    }
    else
    {
      if(lcmb[sl].o[0]=='+')  cp[0]=line[lcmb[sl].l[0]].p1;
      else                    cp[0]=line[lcmb[sl].l[0]].p2;
    }
  }
  else if (surf[nr].typ[0]=='l')
  {
    if (surf[nr].o[0]=='-')
    {
      cp[0]=line[surf[nr].l[0]].p2;
    }
    else
    {
      cp[0]=line[surf[nr].l[0]].p1;
    }
  }
  else { errMsg (" ERROR in orientSurf, surf.typ:%1c not known\n", surf[nr].typ[j]); exit(-1);}

  /* suche den anfangspunkt aller weiteren linien oder lcmbs der surf */
  for (j=1; j<surf[nr].nl; j++)
  {
    sl=surf[nr].l[j];
    if (surf[nr].typ[j]=='c')
    {
      cl=lcmb[sl].nl-1;
      if (surf[nr].o[j]=='-')
      {
        if(lcmb[sl].o[cl]=='+') cp[j]=line[lcmb[sl].l[cl]].p2;
        else                    cp[j]=line[lcmb[sl].l[cl]].p1;
      }
      else
      {
        if(lcmb[sl].o[0]=='+')  cp[j]=line[lcmb[sl].l[0]].p1;
        else                    cp[j]=line[lcmb[sl].l[0]].p2;
      }
    }
    else if (surf[nr].typ[j]=='l')
    {
      if (surf[nr].o[j]=='-')
      {
        cp[j]=line[surf[nr].l[j]].p2;
      }
      else
      {
        cp[j]=line[surf[nr].l[j]].p1;
      }
    }
    else { errMsg (" ERROR in orientSurf, surf.typ:%1c not known\n", surf[nr].typ[j]); exit(-1);}
  }

  /* berechnung des CG, koordinaten werden descaliert gespeichert */
  /* sonst muesste bei jeder aenderung von scale orientSet() ablaufen */
  surf[nr].cx=surf[nr].cy=surf[nr].cz=0.;
  surf[nr].cp=cp;
  for (i=0; i<surf[nr].nl; i++)
  {  
    surf[nr].cx+=(point[cp[i]].px* scale->w+scale->x)/surf[nr].nl;
    surf[nr].cy+=(point[cp[i]].py* scale->w+scale->y)/surf[nr].nl;
    surf[nr].cz+=(point[cp[i]].pz* scale->w+scale->z)/surf[nr].nl;
  }

  return(err);
}


/* ordnet die Flaechen eines Bodies endsprechend der Topologiedefinition */
/* in: */
/*   nr                                   body-index */
/*   cp[SURFS_PER_BODY][EDGES_PER_SURF];  corner points der surfs */
/* out:   */
/*   return 1 if successfull or -1 if not */
int ori7SidedBody( int nr, int **cp )
{
  int i,j,k;
  int s, p0=0, p1=0, sl=0, cl, cl1, p0_s4;
  int surfIndex[7];           /* index einer surf, Topologieabh. gespeichert */
  int body_ns[7];             /* speichert die pos innerhalb der body definition */
  char  surfOri[7];    /* orientierung  */
  double scg[3], bcg[3];
  double v0[3], v1[3], v2[3];
  double fi;

  /* 7surfs: surf-order im surf-kreuz: 0 m, 1 g, 2 lo, 3 l, 4 u, 5 r, 6 ro */
  /* startsurf ist 0, 4 haengt an v01, 5 an v12, 6 an v23, 2 an v34, 3 an v40, 1 bleibt uebrig */  
  /* v01 ist der Vektor von cp[][0] nach cp[][1]  */

  /* zuerst wird die orientierung der ersten surf willkuerlich auf + gesetzt. */
  /* alle anderen werden relativ zur ersten orientiert. wenn die reihenfolge dann feststeht */
  /* erfolgt eine berechnung der orientierungen der surfs, so das die meisten aus dem body zeigen. */
  /* dazu wird der winkel zwischen surf-normalenvektor und surfCG-bodyCG-Vektor berechnet */

  /* suche eine geeignete bez.surf (0), diese muss 5 seiten haben */
  for (i=0; i<body[nr].ns; i++)
  {
    if (surf[body[nr].s[i]].nl==5)
    { 
      sl=i;            /* nr der bezugs surface im body-raum (0-6) */
      goto sfound;
    }
  }
  sfound:;

  k=0;
#if TEST
      errMsg (" in orientBody, side %d sl:%d ori always:1 surf:%s \n",
      k, sl, surf[body[nr].s[sl]].name);   
#endif
  s=body[nr].s[sl];
  surfIndex[k]=s;
  body_ns[k]=sl;
  surfOri[k]=1;


  k=6;  /* laufende nr der zu bestimmenden surf */
  for (i=0; i<body[nr].ns; i++)
  {
    s=body[nr].s[i];
    p0=p1=-1;
    if(i!=sl)
    {
      for (j=0; j<surf[s].nl; j++)
      {
        if( cp[i][j]==cp[sl][2] ) p0=j;
        if( cp[i][j]==cp[sl][3] ) p1=j;
      }
      if ((p0>-1)&&(p1>-1)) break;
    }
  }
  if (i>=body[nr].ns)
  { errMsg (" ERROR in orientBody, side %d from body:%s could not be located \n",
      k, body[nr].name); return(-1);
  }
#if TEST
      errMsg (" in orientBody, side %d  surf:%s \n",
      k, surf[s].name);   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;
  /* surfori relativ zur surf0 */
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  if(p0<p1) surfOri[k]=0 ;
  else      surfOri[k]=1 ;


  k=2;  /* laufende nr der zu bestimmenden surf */
  for (i=0; i<body[nr].ns; i++)
  {
    s=body[nr].s[i];
    p0=p1=-1;
    if(i!=sl)
    {
      for (j=0; j<surf[s].nl; j++)
      {
        if( cp[i][j]==cp[sl][3] ) p0=j;
        if( cp[i][j]==cp[sl][4] ) p1=j;
      }
      if ((p0>-1)&&(p1>-1)) break;
    }
  }
  if (i>=body[nr].ns)
  { errMsg (" ERROR in orientBody, side %d from body:%s could not be located \n",
      k, body[nr].name); return(-1);
  }
#if TEST
      errMsg (" in orientBody, side %d  surf:%s \n",
      k, surf[s].name);   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;
  /* surfori relativ zur surf0 */
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  if(p0<p1) surfOri[k]=0 ;
  else      surfOri[k]=1 ;


  k=3;  /* laufende nr der zu bestimmenden surf */
  for (i=0; i<body[nr].ns; i++)
  {
    if(i!=sl)
    {
      s=body[nr].s[i];
      p0=p1=-1;
      for (j=0; j<surf[s].nl; j++)
      {
        if( cp[i][j]==cp[sl][4] ) p0=j;
        if( cp[i][j]==cp[sl][0] ) p1=j;
      }
      if ((p0>-1)&&(p1>-1)) break;
    }
  }
  if (i>=body[nr].ns)
  { errMsg (" ERROR in orientBody, side %d from body:%s could not be located \n",
      k, body[nr].name); return(-1);
  }
#if TEST
      errMsg (" in orientBody, side %d  surf:%s \n",
      k, surf[s].name);   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  if(p0<p1) surfOri[k]=0 ;
  else      surfOri[k]=1 ;


  k=5;  /* laufende nr der zu bestimmenden surf */
  for (i=0; i<body[nr].ns; i++)
  {
    s=body[nr].s[i];
    p0=p1=-1;
    if(i!=sl)
    {
      for (j=0; j<surf[s].nl; j++)
      {
        if( cp[i][j]==cp[sl][1] ) p0=j;
        if( cp[i][j]==cp[sl][2] ) p1=j;
      }
      if ((p0>-1)&&(p1>-1)) break;
    }
  }
  if (i>=body[nr].ns)
  { errMsg (" ERROR in orientBody, side %d from body:%s could not be located \n",
      k, body[nr].name); return(-1);
  }
#if TEST
      errMsg (" in orientBody, side %d  surf:%s \n",
      k, surf[s].name);   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  if(p0<p1) surfOri[k]=0 ;
  else      surfOri[k]=1 ;


  k=4;  /* laufende nr der zu bestimmenden surf    */
  for (i=0; i<body[nr].ns; i++)
  {
    s=body[nr].s[i];
    p0=p1=-1;
    if(i!=sl)
    {
      for (j=0; j<surf[s].nl; j++)
      {
        if( cp[i][j]==cp[sl][0] ) p0=j;
        if( cp[i][j]==cp[sl][1] ) p1=j;
      }
      if ((p0>-1)&&(p1>-1)) break;
    }
  }
  if (i>=body[nr].ns)
  { errMsg (" ERROR in orientBody, side %d from body:%s could not be located \n",
      k, body[nr].name); return(-1);
  }
#if TEST
      errMsg (" in orientBody, side %d  surf:%s \n",
      k, surf[s].name);   
#endif
  p0_s4=p0;
  surfIndex[k]=s;
  body_ns[k]=i;
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  if(p0<p1) surfOri[k]=0 ;
  else      surfOri[k]=1 ;


  k=1;  /* laufende nr der zu bestimmenden surf                            */
  surfIndex[k]=-1;
  /* suche die noch nicht zugewiesene surf */
  for (i=0; i<body[nr].ns; i++)
  {
    cl=0;
    s=body[nr].s[i];
    for (j=0; j<body[nr].ns; j++) { if(surfIndex[j]==s) cl=1; }
    if(!cl) break;
  }
#if TEST
      errMsg (" in orientBody, side %d  surf:%s \n",
      k, surf[s].name);   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;

  /* bestimme die orientierung mit hilfe der seite nr.4 */
  /* wenn der drehsinn der surf mit surf 4 uebereinstimmt wird die ori von 4 uebernommen */
  /* dafuer muessen die anschlusspunkte gefunden werden. p0 und p1 sind von surf 4 bekannt, */
  /* die gegenueberliegenden werden gesucht */
  if (surfOri[4]==surfOri[0])
  {
    cl=p0_s4+1;
    if (cl>=surf[surfIndex[4]].nl) cl=0;
    cl1=cl+1;
    if (cl1>=surf[surfIndex[4]].nl) cl1=0;
  }
  else
  {
    cl=p0_s4+2;
    if (cl>=surf[surfIndex[4]].nl) cl-=surf[surfIndex[4]].nl;
    cl1=cl+1;
    if (cl1>=surf[surfIndex[4]].nl) cl1-=surf[surfIndex[4]].nl;
  }

  /* suche die anschlusspunkte zu surf 1 */
  p0=p1=-1;
  for (j=0; j<surf[s].nl; j++)
  {
    if( cp[i][j]==cp[body_ns[4]][cl] ) p0=j;
    if( cp[i][j]==cp[body_ns[4]][cl1] ) p1=j;
  }
  if((p0==4)&&(p1==0)) p1=5;
  else if((p1==4)&&(p0==0)) p0=5;
  /* wenn die surf die gleiche drehrichtung hat wie surf4: */
  if (p0>p1) surfOri[1]=surfOri[4];
  else if(surfOri[4]==1) surfOri[1]=0;
  else surfOri[1]=1;


  /* die orientierungen aller surfs werden nun mit hilfe des schwerpunktes des bodies ueberprueft */
  /* wenn die mehrzahl der surfnormalen aus dem body zeigt, ist alles ok, sonst invertieren */
  cl=0;
  for (i=0; i<body[nr].ns; i++)
  {
    s = surfIndex[i];
    sl= body_ns[i];
    scg[0]=surf[s].cx;
    scg[1]=surf[s].cy;
    scg[2]=surf[s].cz;
    bcg[0]=body[nr].cx;
    bcg[1]=body[nr].cy;
    bcg[2]=body[nr].cz;
    v0[0]=point[cp[sl][0]].px;
    v0[1]=point[cp[sl][0]].py;
    v0[2]=point[cp[sl][0]].pz;
    v1[0]=point[cp[sl][1]].px;
    v1[1]=point[cp[sl][1]].py;
    v1[2]=point[cp[sl][1]].pz;
    v2[0]=point[cp[sl][2]].px;
    v2[1]=point[cp[sl][2]].py;
    v2[2]=point[cp[sl][2]].pz;
    fi=angleSurfBody( v0, v1, v2, scg, bcg );
    if(surfOri[i])
    {
      if(fi>0.) cl-- ;
      else      cl++ ;
    }
    else
    {
      if(fi>0.) cl++ ;
      else      cl-- ;
    }
  }

  /* kontrolle ob der body zu invertieren ist */
  j=getSetNr(specialset->flpb);
  if(j>-1)
    for( i=0; i<set[j].anz_b; i++)
    {
      if(nr==set[j].body[i]) { if(cl) cl*=-1; else cl=-1; break; }
    }

  /* wenn cl<0: vertauschen von surf3 mit surf5  */
  /* und vertauschen von surf2 mit surf6 und invertieren aller surfOri  */
  if(cl<0)
  {
    cl1=surfOri[3];
    surfOri[3]=surfOri[5];
    surfOri[5]=cl1;
    cl1=surfIndex[3];
    surfIndex[3]=surfIndex[5];
    surfIndex[5]=cl1;

    cl1=surfOri[2];
    surfOri[2]=surfOri[6];
    surfOri[6]=cl1;
    cl1=surfIndex[2];
    surfIndex[2]=surfIndex[6];
    surfIndex[6]=cl1;
  }

  /* define the body-structure */
  for (i=0; i<body[nr].ns; i++)
  {
    body[nr].s[i]=surfIndex[i];
    if((cl>0)&&(surfOri[i])) body[nr].o[i]='+';
    else if((cl<0)&&(surfOri[i])) body[nr].o[i]='-';
    else if((cl>0)&&(!surfOri[i])) body[nr].o[i]='-';
    else if((cl<0)&&(!surfOri[i])) body[nr].o[i]='+';
    if(printFlag) printf (" %1c %s", body[nr].o[i], surf[ body[nr].s[i] ].name );
  }
  if(printFlag) printf("\n");
  return(1);
}


/* ordnet die Flaechen eines Bodies endsprechend der Topologiedefinition */
/* in: */
/*   nr                                   body-index */
/*   cp[SURFS_PER_BODY][EDGES_PER_SURF];  corner points der surfs */
/* out:   */
/*   return 1 if successfull or -1 if not */
int ori6SidedBody( int nr, int **cp )
{
  int i,j,k,n;
  int s, p0=0, p1=0, sl=0, cl, cl1, ipuf, p0_s4;
  int surfIndex[6];           /* index einer surf, Topologieabh. gespeichert */
  int body_ns[6];             /* speichert die pos innerhalb der body definition */
  char  surfOri[6];    /* orientierung  */
  int   p_colapse[6];
  double scg[3], bcg[3];
  double v0[3], v1[3], v2[3];
  double fi;

  /* 6surfs: surf-order im surf-kreuz: 0 m, 1 g, 2 o, 3 l, 4 u, 5 r */
  /* startsurf ist 0, 4 haengt an v01, 5 an v12, 2 an v23, 3 an v30 , 1 bleibt uebrig */  
  /* v01 ist der Vektor von cp[][0] nach cp[][1]  */

  /* ACHTUNG: in meshSet() wird die Reienfolge der Body-Corner-Points (bcp) anders definiert */
  /* dort gilt bcp 0-3 werden durch die Mastersurf (0) festgelegt (orientabhaengig)  */
  /*
    if ( body[b_indx].o[0]=='+')
    {
      bcp[0]=scp[0][3];
      bcp[1]=scp[0][2];
      bcp[2]=scp[0][1];
      bcp[3]=scp[0][0];
    }
    else if (body[b_indx].o[0]=='-')
    {
      bcp[0]=scp[0][2];
      bcp[1]=scp[0][3];
      bcp[2]=scp[0][0];
      bcp[3]=scp[0][1];
      }
  */

  /* zuerst wird die orientierung der ersten surf willkuerlich auf + gesetzt. */
  /* alle anderen werden relativ zur ersten orientiert. wenn die reihenfolge dann feststeht */
  /* erfolgt eine berechnung der orientierungen der surfs, so das die meisten aus dem body zeigen. */
  /* dazu wird der winkel zwischen surf-normalenvektor und surfCG-bodyCG-Vektor berechnet */

  /* suche eine geeignete bez.surf (0), fuer einen gewoehnl. brick ist jede Recht */
  /* fuer ein brick mit kollabierter seite darf die bez.surf keinen eckpunkt der kol-*/
  /* labierten surf enthalten. */

  /* suche punkte an den kollabierten seiten */
  n=0;
  for (i=0; i<body[nr].ns; i++)
  {
    if (cp[i][0]==cp[i][1]) {  p_colapse[n]=cp[i][0]; n++; }
    if (cp[i][1]==cp[i][2]) {  p_colapse[n]=cp[i][1]; n++; }
    if (cp[i][2]==cp[i][3]) {  p_colapse[n]=cp[i][2]; n++; }
    if (cp[i][3]==cp[i][0]) {  p_colapse[n]=cp[i][3]; n++; }
  }

  /* suche die seite die keine punkte kollabierter seiten enthaelt */
  for (i=0; i<body[nr].ns; i++)
  {
    ipuf=1;
    for (j=0; j<surf[body[nr].s[i]].nl; j++)
    {
      for (k=0; k<n; k++)
      {
        if (cp[i][j]==p_colapse[k]) ipuf=0;
      }
    }
    if (ipuf)
    { 
      sl=i;            /* nr der bezugs surface im body-raum (0-5) */
      goto sfound;
    }
  }
  sfound:;

  k=0;
#if TEST
      errMsg (" in orientBody, side %d sl:%d ori always:1 surf:%s \n",
      k, sl, surf[body[nr].s[sl]].name);   
#endif
  s=body[nr].s[sl];
  surfIndex[k]=s;
  body_ns[k]=sl;
  surfOri[k]=1;


  k=2;  /* laufende nr der zu bestimmenden surf */
  for (i=0; i<body[nr].ns; i++)
  {
    s=body[nr].s[i];
    p0=p1=-1;
    if(i!=sl)
    {
      for (j=0; j<surf[s].nl; j++)
      {
        if( cp[i][j]==cp[sl][2] ) p0=j;
        if( cp[i][j]==cp[sl][3] ) p1=j;
      }
      if ((p0>-1)&&(p1>-1)) break;
    }
  }
  if (i>=body[nr].ns)
  { errMsg (" ERROR in orientBody, side %d from body:%s could not be located \n",
      k, body[nr].name); return(-1);
  }
#if TEST
      printf (" in orientBody, side:%d surf:%s p0:%d p1:%d %s %s",
      k, surf[s].name,p0,p1, point[cp[i][p0]].name, point[cp[i][p1]].name );   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;
  /* surfori relativ zur surf0 */
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  if(p0<p1) surfOri[k]=0 ;
  else      surfOri[k]=1 ;
#if TEST
      printf (" %d\n", surfOri[k] );   
#endif


  k=3;  /* laufende nr der zu bestimmenden surf */
  for (i=0; i<body[nr].ns; i++)
  {
    if(i!=sl)
    {
      s=body[nr].s[i];
      p0=p1=-1;
      for (j=0; j<surf[s].nl; j++)
      {
        if( cp[i][j]==cp[sl][3] ) p0=j;
        if( cp[i][j]==cp[sl][0] ) p1=j;
      }
      if ((p0>-1)&&(p1>-1)) break;
    }
  }
  if (i>=body[nr].ns)
  { errMsg (" ERROR in orientBody, side %d from body:%s could not be located \n",
      k, body[nr].name); return(-1);
  }
#if TEST
      printf (" in orientBody, side:%d surf:%s p0:%d p1:%d %s %s",
      k, surf[s].name,p0,p1, point[cp[i][p0]].name, point[cp[i][p1]].name );   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  if(p0<p1) surfOri[k]=0 ;
  else      surfOri[k]=1 ;
#if TEST
      printf (" %d\n", surfOri[k] );   
#endif


  k=5;  /* laufende nr der zu bestimmenden surf */
  for (i=0; i<body[nr].ns; i++)
  {
    s=body[nr].s[i];
    p0=p1=-1;
    if(i!=sl)
    {
      for (j=0; j<surf[s].nl; j++)
      {
        if( cp[i][j]==cp[sl][1] ) p0=j;
        if( cp[i][j]==cp[sl][2] ) p1=j;
      }
      if ((p0>-1)&&(p1>-1)) break;
    }
  }
  if (i>=body[nr].ns)
  { errMsg (" ERROR in orientBody, side %d from body:%s could not be located \n",
      k, body[nr].name); return(-1);
  }
#if TEST
      printf (" in orientBody, side:%d surf:%s p0:%d p1:%d %s %s",
      k, surf[s].name,p0,p1, point[cp[i][p0]].name, point[cp[i][p1]].name );   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  if(p0<p1) surfOri[k]=0 ;
  else      surfOri[k]=1 ;
#if TEST
      printf (" %d\n", surfOri[k] );   
#endif


  k=4;  /* laufende nr der zu bestimmenden surf    */
  for (i=0; i<body[nr].ns; i++)
  {
    s=body[nr].s[i];
    p0=p1=-1;
    if(i!=sl)
    {
      for (j=0; j<surf[s].nl; j++)
      {
        if( cp[i][j]==cp[sl][0] ) p0=j;
        if( cp[i][j]==cp[sl][1] ) p1=j;
      }
      if ((p0>-1)&&(p1>-1)) break;
    }
  }
  if (i>=body[nr].ns)
  { errMsg (" ERROR in orientBody, side %d from body:%s could not be located \n",
      k, body[nr].name); return(-1);
  }
#if TEST
      printf (" in orientBody, side:%d surf:%s p0:%d p1:%d %s %s",
      k, surf[s].name,p0,p1, point[cp[i][p0]].name, point[cp[i][p1]].name );   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;
  p0_s4=p0;
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  if(p0<p1) surfOri[k]=0 ;
  else      surfOri[k]=1 ;
#if TEST
      printf (" %d\n", surfOri[k] );   
#endif


  k=1;  /* laufende nr der zu bestimmenden surf                            */
  surfIndex[k]=-1;
  /* suche die noch nicht zugewiesene surf */
  for (i=0; i<body[nr].ns; i++)
  {
    cl=0;
    s=body[nr].s[i];
    for (j=0; j<body[nr].ns; j++) { if(surfIndex[j]==s) cl=1; }
    if(!cl) break;
  }
  surfIndex[k]=s;
  body_ns[k]=i;

  /* bestimme die orientierung mit hilfe der seite nr.4 */
  /* wenn der drehsinn der surf mit surf 4 uebereinstimmt wird die ori von 4 uebernommen */
  /* dafuer muessen die anschlusspunkte gefunden werden. p0 und p1 sind von surf 4 bekannt, */
  /* die gegenueberliegenden werden gesucht */
  if (surfOri[4]==surfOri[0])
  {
    cl=p0_s4+1;
    if (cl>=surf[surfIndex[4]].nl) cl=0;
    cl1=cl+1;
    if (cl1>=surf[surfIndex[4]].nl) cl1=0;
  }
  else
  {
    cl=p0_s4+2;
    if (cl>=surf[surfIndex[4]].nl) cl-=surf[surfIndex[4]].nl;
    cl1=cl+1;
    if (cl1>=surf[surfIndex[4]].nl) cl1-=surf[surfIndex[4]].nl;
  }

  /* suche die anschlusspunkte zu surf 1 */
  p0=p1=-1;
  for (j=0; j<surf[s].nl; j++)
  {
    if( cp[i][j]==cp[body_ns[4]][cl] ) p0=j;
    if( cp[i][j]==cp[body_ns[4]][cl1] ) p1=j;
  }
#if TEST
      printf (" in orientBody, side:%d surf:%s p0:%d p1:%d %s %s",
      k, surf[s].name,p0,p1, point[cp[i][p0]].name, point[cp[i][p1]].name );   
#endif
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  /* wenn die surf die gleiche drehrichtung hat wie surf4: */
  if (p0>p1) surfOri[1]=surfOri[4];
  else if(surfOri[4]==1) surfOri[1]=0;
  else surfOri[1]=1;
#if TEST
      printf (" %d\n", surfOri[k] );   
#endif

  /* die orientierungen aller surfs werden nun mit hilfe des schwerpunktes des bodies ueberprueft */
  /* wenn die mehrzahl der surfnormalen aus dem body zeigt, ist alles ok, sonst invertieren */
  cl=0;
  for (i=0; i<body[nr].ns; i++)
  {
    s = surfIndex[i];
    sl= body_ns[i];
    scg[0]=surf[s].cx;
    scg[1]=surf[s].cy;
    scg[2]=surf[s].cz;
    bcg[0]=body[nr].cx;
    bcg[1]=body[nr].cy;
    bcg[2]=body[nr].cz;
    v0[0]=point[cp[sl][0]].px;
    v0[1]=point[cp[sl][0]].py;
    v0[2]=point[cp[sl][0]].pz;
    v1[0]=point[cp[sl][1]].px;
    v1[1]=point[cp[sl][1]].py;
    v1[2]=point[cp[sl][1]].pz;
    v2[0]=point[cp[sl][2]].px;
    v2[1]=point[cp[sl][2]].py;
    v2[2]=point[cp[sl][2]].pz;
    fi=angleSurfBody( v0, v1, v2, scg, bcg );
    /* plus zweitem winkel da eine seite concav sein koennte */ 
    v0[0]=point[cp[sl][1]].px;
    v0[1]=point[cp[sl][1]].py;
    v0[2]=point[cp[sl][1]].pz;
    v1[0]=point[cp[sl][2]].px;
    v1[1]=point[cp[sl][2]].py;
    v1[2]=point[cp[sl][2]].pz;
    v2[0]=point[cp[sl][3]].px;
    v2[1]=point[cp[sl][3]].py;
    v2[2]=point[cp[sl][3]].pz;
    fi+=angleSurfBody( v0, v1, v2, scg, bcg );
    /* plus zweitem winkel da eine seite concav sein koennte */ 
    v0[0]=point[cp[sl][2]].px;
    v0[1]=point[cp[sl][2]].py;
    v0[2]=point[cp[sl][2]].pz;
    v1[0]=point[cp[sl][3]].px;
    v1[1]=point[cp[sl][3]].py;
    v1[2]=point[cp[sl][3]].pz;
    v2[0]=point[cp[sl][0]].px;
    v2[1]=point[cp[sl][0]].py;
    v2[2]=point[cp[sl][0]].pz;
    fi+=angleSurfBody( v0, v1, v2, scg, bcg );
    /* plus zweitem winkel da eine seite concav sein koennte */ 
    v0[0]=point[cp[sl][3]].px;
    v0[1]=point[cp[sl][3]].py;
    v0[2]=point[cp[sl][3]].pz;
    v1[0]=point[cp[sl][0]].px;
    v1[1]=point[cp[sl][0]].py;
    v1[2]=point[cp[sl][0]].pz;
    v2[0]=point[cp[sl][1]].px;
    v2[1]=point[cp[sl][1]].py;
    v2[2]=point[cp[sl][1]].pz;
    fi+=angleSurfBody( v0, v1, v2, scg, bcg );
    if(surfOri[i])
    {
      if(fi>0.) cl-- ;
      else      cl++ ;
    }
    else
    {
      if(fi>0.) cl++ ;
      else      cl-- ;
    }
#if TEST
    printf(" ori:%d cl:%d fi:%lf\n",surfOri[i],cl,fi); 
#endif
  }

  /* kontrolle ob der body zu invertieren ist */
  j=getSetNr(specialset->flpb);
  if(j>-1)
  {
    for( i=0; i<set[j].anz_b; i++)
    {
      if(nr==set[j].body[i]) { if(cl!=0) cl*=-1; else cl=-1; break; }
    }
  }


  /* wenn cl<0: vertauschen von surf3 mit surf5 und invertieren aller surfOri  */
  if(cl<0)
  {
    cl1=surfOri[3];
    surfOri[3]=surfOri[5];
    surfOri[5]=cl1;
    cl1=surfIndex[3];
    surfIndex[3]=surfIndex[5];
    surfIndex[5]=cl1;
  }

  /* define the body-structure */
  for (i=0; i<body[nr].ns; i++)
  {
    body[nr].s[i]=surfIndex[i];
    if((cl>=0)&&(surfOri[i])) body[nr].o[i]='+';
    else if((cl<0)&&(surfOri[i])) body[nr].o[i]='-';
    else if((cl>=0)&&(!surfOri[i])) body[nr].o[i]='-';
    else if((cl<0)&&(!surfOri[i])) body[nr].o[i]='+';
    /* if(surf[ body[nr].s[i] ].name[3]=='4') body[nr].o[i]='-'; */
    if(printFlag) printf (" %1c %s", body[nr].o[i], surf[ body[nr].s[i] ].name );
  }
  if(printFlag) printf("\n");
  return(1);
}


/* ordnet die Flaechen eines Bodies endsprechend der Topologiedefinition */
/* in: */
/*   nr                                   body-index */
/*   cp[SURFS_PER_BODY][EDGES_PER_SURF];  corner points der surfs */
/* out:   */
/*   return 1 if successfull or -1 if not */
int ori5SidedBody( int nr, int **cp )
{
  int i,j,k;
  int   s, p0=0, p1=0, sl=0, cl, cl1, p0_s4;
  int   surfIndex[5];           /* index einer surf, Topologieabh. gespeichert */
  int   body_ns[5];             /* speichert die pos innerhalb der body definition */
  char  surfOri[5];    /* orientierung  */
  double scg[3], bcg[3];
  double v0[3], v1[3], v2[3];
  double fi;

  /* surfs: surf-order im surf-kreuz: 0 m, 1 g, 2 r, 3 l, 4 u */
  /* startsurf ist 0, 4 haengt an v01, 2 an v12, 3 an v20 , 1 bleibt uebrig */  
  /* v01 ist der Vektor von cp[][0] nach cp[][1]  */

  /* zuerst wird die orientierung der ersten surf willkuerlich auf + gesetzt. */
  /* alle anderen werden relativ zur ersten orientiert. wenn die reihenfolge dann feststeht */
  /* erfolgt eine berechnung der orientierungen der surfs, so das die meisten aus dem body zeigen. */
  /* dazu wird der winkel zwischen surf-normalenvektor und surfCG-bodyCG-Vektor berechnet */

  /* suche eine geeignete bez.surf (0), es muss eine 3-seitige sein. */
  for (i=0; i<body[nr].ns; i++)
  {
    if (surf[body[nr].s[i]].nl==3)
    { 
      sl=i;            /* nr der bezugs surface im body-raum (0-4) */
      goto sfound;
    }
  }
  sfound:;

  k=0;
#if TEST
      errMsg (" in orientBody, side %d sl:%d ori always:1 surf:%s \n",
      k, sl, surf[body[nr].s[sl]].name);   
#endif
  s=body[nr].s[sl];
  surfIndex[k]=s;
  body_ns[k]=sl;
  surfOri[k]=1;


  k=2;  /* laufende nr der zu bestimmenden surf */
  for (i=0; i<body[nr].ns; i++)
  {
    s=body[nr].s[i];
    p0=p1=-1;
    if(i!=sl)
    {
      for (j=0; j<surf[s].nl; j++)
      {
        if( cp[i][j]==cp[sl][1] ) p0=j;
        if( cp[i][j]==cp[sl][2] ) p1=j;
      }
      if ((p0>-1)&&(p1>-1)) break;
    }
  }
  if (i>=body[nr].ns)
  { errMsg (" ERROR in orientBody, side %d from body:%s could not be located \n",
      k, body[nr].name); return(-1);
  }
#if TEST
      errMsg (" in orientBody, side %d  surf:%s \n",
      k, surf[s].name);   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;
  /* surfori relativ zur surf0 */
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  if(p0<p1) surfOri[k]=0 ;
  else      surfOri[k]=1 ;


  k=3;  /* laufende nr der zu bestimmenden surf */
  for (i=0; i<body[nr].ns; i++)
  {
    if(i!=sl)
    {
      s=body[nr].s[i];
      p0=p1=-1;
      for (j=0; j<surf[s].nl; j++)
      {
        if( cp[i][j]==cp[sl][2] ) p0=j;
        if( cp[i][j]==cp[sl][0] ) p1=j;
      }
      if ((p0>-1)&&(p1>-1)) break;
    }
  }
  if (i>=body[nr].ns)
  { errMsg (" ERROR in orientBody, side %d from body:%s could not be located \n",
      k, body[nr].name); return(-1);
  }
#if TEST
      errMsg (" in orientBody, side %d  surf:%s \n",
      k, surf[s].name);   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  if(p0<p1) surfOri[k]=0 ;
  else      surfOri[k]=1 ;


  k=4;  /* laufende nr der zu bestimmenden surf    */
  for (i=0; i<body[nr].ns; i++)
  {
    s=body[nr].s[i];
    p0=p1=-1;
    if(i!=sl)
    {
      for (j=0; j<surf[s].nl; j++)
      {
        if( cp[i][j]==cp[sl][0] ) p0=j;
        if( cp[i][j]==cp[sl][1] ) p1=j;
      }
      if ((p0>-1)&&(p1>-1)) break;
    }
  }
  if (i>=body[nr].ns)
  { errMsg (" ERROR in orientBody, side %d from body:%s could not be located \n",
      k, body[nr].name); return(-1);
  }
#if TEST
      errMsg (" in orientBody, side %d  surf:%s \n",
      k, surf[s].name);   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;
  p0_s4=p0;
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  if(p0<p1) surfOri[k]=0 ;
  else      surfOri[k]=1 ;


  k=1;  /* laufende nr der zu bestimmenden surf                            */
  surfIndex[k]=-1;
  /* suche die noch nicht zugewiesene surf */
  for (i=0; i<body[nr].ns; i++)
  {
    cl=0;
    s=body[nr].s[i];
    for (j=0; j<body[nr].ns; j++) { if(surfIndex[j]==s) cl=1; }
    if(!cl) break;
  }
#if TEST
      errMsg (" in orientBody, side %d  surf:%s \n",
      k, surf[s].name);   
#endif
  surfIndex[k]=s;
  body_ns[k]=i;

  /* bestimme die orientierung mit hilfe der seite nr.4 */
  /* wenn der drehsinn der surf mit surf 4 uebereinstimmt wird die ori von 4 uebernommen */
  /* dafuer muessen die anschlusspunkte gefunden werden. p0 und p1 sind von surf 4 bekannt, */
  /* die gegenueberliegenden werden gesucht */
  if (surfOri[4]==surfOri[0])
  {
    cl=p0_s4+1;
    if (cl>=surf[surfIndex[4]].nl) cl=0;
    cl1=cl+1;
    if (cl1>=surf[surfIndex[4]].nl) cl1=0;
  }
  else
  {
    cl=p0_s4+2;
    if (cl>=surf[surfIndex[4]].nl) cl-=surf[surfIndex[4]].nl;
    cl1=cl+1;
    if (cl1>=surf[surfIndex[4]].nl) cl1-=surf[surfIndex[4]].nl;
  }

  /* suche die anschlusspunkte zu surf 1 */
  p0=p1=-1;
  for (j=0; j<surf[s].nl; j++)
  {
    if( cp[i][j]==cp[body_ns[4]][cl] ) p0=j;
    if( cp[i][j]==cp[body_ns[4]][cl1] ) p1=j;
  }
  if((p0==3)&&(p1==0)) p1=4;
  else if((p1==3)&&(p0==0)) p0=4;
  /* wenn die surf die gleiche drehrichtung hat wie surf4: */
  if (p0>p1) surfOri[1]=surfOri[4];
  else if(surfOri[4]==1) surfOri[1]=0;
  else surfOri[1]=1;


  /* die orientierungen aller surfs werden nun mit hilfe des schwerpunktes des bodies ueberprueft */
  /* wenn die mehrzahl der surfnormalen aus dem body zeigt, ist alles ok, sonst invertieren */
  cl=0;
  for (i=0; i<body[nr].ns; i++)
  {
    s = surfIndex[i];
    sl= body_ns[i];
    scg[0]=surf[s].cx;
    scg[1]=surf[s].cy;
    scg[2]=surf[s].cz;
    bcg[0]=body[nr].cx;
    bcg[1]=body[nr].cy;
    bcg[2]=body[nr].cz;
    v0[0]=point[cp[sl][0]].px;
    v0[1]=point[cp[sl][0]].py;
    v0[2]=point[cp[sl][0]].pz;
    v1[0]=point[cp[sl][1]].px;
    v1[1]=point[cp[sl][1]].py;
    v1[2]=point[cp[sl][1]].pz;
    v2[0]=point[cp[sl][2]].px;
    v2[1]=point[cp[sl][2]].py;
    v2[2]=point[cp[sl][2]].pz;
    fi=angleSurfBody( v0, v1, v2, scg, bcg );
    if(surfOri[i])
    {
      if(fi>0.) cl-- ;
      else      cl++ ;
    }
    else
    {
      if(fi>0.) cl++ ;
      else      cl-- ;
    }
  }

  /* kontrolle ob der body zu invertieren ist */
  j=getSetNr(specialset->flpb);
  if(j>-1)
    for( i=0; i<set[j].anz_b; i++)
    {
      if(nr==set[j].body[i]) { if(cl) cl*=-1; else cl=-1; break; }
    }

  /* wenn cl<0: vertauschen von surf3 mit surf2 und invertieren aller surfOri  */
  if(cl<0)
  {
    cl1=surfOri[3];
    surfOri[3]=surfOri[2];
    surfOri[2]=cl1;
    cl1=surfIndex[3];
    surfIndex[3]=surfIndex[2];
    surfIndex[2]=cl1;
  }

  /* define the body-structure */
  for (i=0; i<body[nr].ns; i++)
  {
    body[nr].s[i]=surfIndex[i];
    if((cl>0)&&(surfOri[i])) body[nr].o[i]='+';
    else if((cl<0)&&(surfOri[i])) body[nr].o[i]='-';
    else if((cl>0)&&(!surfOri[i])) body[nr].o[i]='-';
    else if((cl<0)&&(!surfOri[i])) body[nr].o[i]='+';
    if(printFlag) printf (" %1c %s", body[nr].o[i], surf[ body[nr].s[i] ].name );
  }
  if(printFlag) printf("\n");
  return(1);
}


int orientBody( int nr )
{
  register int i,j=0,n;
  int   s, sl, cl, err=0;
  int   **cp;                  /* corner points of the surfs */


  if( (cp=(int **)malloc( (body[nr].ns)*sizeof(int *) ) )==NULL)    
  { printf(" ERROR: malloc failure in orientBody()\n"); return(-1); }
  for(i=0; i<body[nr].ns; i++)
  {
    if( (cp[i]=(int *)malloc(surf[body[nr].s[i]].nl*sizeof(int) ) )==NULL)    
    { printf(" ERROR: malloc failure in orientBody()\n"); return(-1); }
  }

  if(printFlag) printf (" orient body:%s \n", body[nr].name);

  /* suche die Eckknoten der surfaces */
  /* beruecksichtige dabei die orientierung der linien oder lcmbs */
  /* so das die punktefolge der linienfolge entspricht */
  for (i=0; i<body[nr].ns; i++)
  {
    s=body[nr].s[i];

    /* suche den ersten punkt der ersten linie oder lcmb der surf */
    if(surf[s].nl<1)
    { printf(" ERROR: failure in orientBody()\n"); return(-1); }
    sl=surf[s].l[0];
    if (surf[s].typ[0]=='c')
    {
      cl=lcmb[sl].nl-1;
      if (surf[s].o[0]=='-')
      {
        if(lcmb[sl].o[cl]=='+') cp[i][0]=line[lcmb[sl].l[cl]].p2;
        else                    cp[i][0]=line[lcmb[sl].l[cl]].p1;
      }
      else
      {
        if(lcmb[sl].o[0]=='+')  cp[i][0]=line[lcmb[sl].l[0]].p1;
        else                    cp[i][0]=line[lcmb[sl].l[0]].p2;
      }
    }
    else if (surf[s].typ[0]=='l')
    {
      if (surf[s].o[0]=='-')
      {
        cp[i][0]=line[surf[s].l[0]].p2;
      }
      else
      {
        cp[i][0]=line[surf[s].l[0]].p1;
      }
    }
    else { errMsg (" ERROR in orientBody, surf.typ:%1c not known\n", surf[s].typ[j]); exit(-1);}

    /* suche den anfangspunkt aller weiteren linien oder lcmbs der surf */
    for (j=1; j<surf[s].nl; j++)
    {
      sl=surf[s].l[j];
      if (surf[s].typ[j]=='c')
      {
        cl=lcmb[sl].nl-1;
        if (surf[s].o[j]=='-')
        {
          if(lcmb[sl].o[cl]=='+') cp[i][j]=line[lcmb[sl].l[cl]].p2;
          else                    cp[i][j]=line[lcmb[sl].l[cl]].p1;
        }
        else
        {
          if(lcmb[sl].o[0]=='+')  cp[i][j]=line[lcmb[sl].l[0]].p1;
          else                    cp[i][j]=line[lcmb[sl].l[0]].p2;
        }
      }
      else if (surf[s].typ[j]=='l')
      {
        if (surf[s].o[j]=='-')
        {
          cp[i][j]=line[surf[s].l[j]].p2;
        }
        else
        {
          cp[i][j]=line[surf[s].l[j]].p1;
        }
      }
      else { errMsg (" ERROR in orientBody, surf.typ:%1c not known\n", surf[s].typ[j]); exit(-1);}
    }
    /*
    printf ("\n");
    printf ("s:%s ",  surf[s].name );
    for (j=0; j<surf[s].nl; j++)
        printf (" %s", point[cp[i][j]].name);
    printf ("\n");
    */
  }

  /* berechnung des CG, koordinaten werden descaliert gespeichert */
  /* sonst muesste bei jeder aenderung von scale orientSet() ablaufen */
  n=0;
  body[nr].cx=body[nr].cy=body[nr].cz=0.;
  for (i=0; i<body[nr].ns; i++)
  {
    s=body[nr].s[i];
    for (j=0; j<surf[s].nl; j++)
    {
      body[nr].cx+=point[cp[i][j]].px* scale->w+scale->x;
      body[nr].cy+=point[cp[i][j]].py* scale->w+scale->y;
      body[nr].cz+=point[cp[i][j]].pz* scale->w+scale->z;
      n++;
    }
  }
  body[nr].cx/=n;
  body[nr].cy/=n;
  body[nr].cz/=n;


  /* die Eckknoten aller Surfaces sind sortiert, sortiere und orientiere die surfaces */
  /* entsprechend den Anforderungen des Meshers */

  if (body[nr].ns==6)
  { if(ori6SidedBody( nr, cp )==-1) err=-2;}
  else if (body[nr].ns==5)
  { if(ori5SidedBody( nr, cp )==-1) err=-3;}
  else if (body[nr].ns==7)
  { if(ori7SidedBody( nr, cp )==-1) err=-4;}
  else err=-1;
  if( (cp=(int **)malloc( (body[nr].ns)*sizeof(int *) ) )==NULL)    
  { printf(" ERROR: malloc failure in orientBody()\n"); return(-1); }
  for(i=0; i<body[nr].ns; i++)
  {
    if( (cp[i]=(int *)malloc(surf[body[nr].s[i]].nl*sizeof(int) ) )==NULL)    
    { printf(" ERROR: malloc failure in orientBody()\n"); return(-1); }
  }

  for(i=0; i<body[nr].ns; i++) free(cp[i]);
  free(cp);
  return(err);
}


void orientSet( char *record)
{
  int   setNr, flag;
  register int i,j=0;
  int length;
  char setname[MAX_LINE_LENGTH];

  length=sword( record, setname );
  operateAlias( setname, "se" );
  setNr=getSetNr(setname);

  if (setNr<0)
  {
    printf (" ERROR: set:%s does not exist\n", setname);
    return;
  }

  delSet(specialset->uori);

  for (i=0; i<set[setNr].anz_c; i++)
  {
   if(lcmb[set[setNr].lcmb[i]].name !=(char *)NULL)
   {
    if(printFlag) printf (" orient lcmb:%s from setname:%s \n", lcmb[set[setNr].lcmb[i]].name, set[setNr].name);
    flag=orientLcmb( set[setNr].lcmb[i] );
    if (flag<0)
    {
      errMsg(" ERROR: Orientation of lcmb:%s failed\n", lcmb[set[setNr].lcmb[i]].name);
      pre_seta( specialset->uori, "c", lcmb[set[setNr].lcmb[i]].name );
      j++;
    }
   }
  }
  for (i=0; i<set[setNr].anz_s; i++)
  {
   if(surf[set[setNr].surf[i]].name !=(char *)NULL)
   {
    if(printFlag) printf (" orient surf:%s from setname:%s \n", surf[set[setNr].surf[i]].name, set[setNr].name);
    flag=orientSurf( set[setNr].surf[i] );
    if (flag<0)
    {
      errMsg(" ERROR: Orientation of surf:%s failed\n", surf[set[setNr].surf[i]].name);
      pre_seta( specialset->uori, "s", surf[set[setNr].surf[i]].name );
      j++;
    }
   }
  }
  for (i=0; i<set[setNr].anz_b; i++)
  {
   if(body[set[setNr].body[i]].name !=(char *)NULL)
   {
    if(printFlag) printf (" orient body:%s from setname:%s \n", body[set[setNr].body[i]].name, set[setNr].name);
    flag=orientBody( set[setNr].body[i] );
    if (flag<0)
    {
      errMsg(" ERROR: Orientation of body:%s failed\n", body[set[setNr].body[i]].name);
      pre_seta( specialset->uori, "b", body[set[setNr].body[i]].name );
      j++;
    }
   }
  }
  if( j>0) printf("WARNING: %d entities are unoriented, check set:%s\n", j, specialset->uori);
}


