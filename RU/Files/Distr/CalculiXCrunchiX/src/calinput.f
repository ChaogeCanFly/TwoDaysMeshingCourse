!
!     CalculiX - A 3-dimensional finite element program
!              Copyright (C) 1998-2013 Guido Dhondt
!
!     This program is free software; you can redistribute it and/or
!     modify it under the terms of the GNU General Public License as
!     published by the Free Software Foundation(version 2);
!     
!
!     This program is distributed in the hope that it will be useful,
!     but WITHOUT ANY WARRANTY; without even the implied warranty of 
!     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
!     GNU General Public License for more details.
!
!     You should have received a copy of the GNU General Public License
!     along with this program; if not, write to the Free Software
!     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
!
      subroutine calinput(co,nk,kon,ipkon,lakon,nkon,
     &  ne,nodeboun,ndirboun,xboun,nboun,
     &  ipompc,nodempc,coefmpc,nmpc,nmpc_,nodeforc,ndirforc,xforc,nforc,
     &  nforc_,nelemload,sideload,xload,nload,nload_,
     &  nprint,prlab,prset,mpcfree,nboun_,
     &  mei,set,istartset,iendset,ialset,nset,nalset,elcon,nelcon,rhcon,
     &  nrhcon,alcon,nalcon,alzero,t0,t1,
     &  matname,ielmat,orname,orab,ielorien,amname,amta,namta,nam,
     &  nmethod,iamforc,iamload,iamt1,
     &  ithermal,iperturb,istat,istep,nmat,ntmat_,norien,
     &  prestr,iprestr,isolver,fei,veold,tinc,tper,
     &  xmodal,filab,jout,nlabel,idrct,jmax,
     &  tmin,tmax,iexpl,alpha,iamboun,plicon,nplicon,plkcon,
     &  nplkcon,iplas,npmat_,mi,nk_,trab,inotr,ntrans,ikboun,
     &  ilboun,ikmpc,ilmpc,ics,dcs,ncs_,namtot_,cs,nstate_,ncmat_,iumat,
     &  mcs,labmpc,iponor,xnor,knor,thickn,thicke,ikforc,ilforc,
     &  offset,iponoel,inoel,rig,infree,nshcon,shcon,cocon,ncocon,
     &  physcon,nflow,ctrl,maxlenmpc,ne1d,
     &  ne2d,nener,vold,nodebounold,ndirbounold,xbounold,
     &  xforcold,xloadold,t1old,eme,sti,ener,xstate,jobnamec,
     &  nnn,irstrt,ttime,qaold,output,typeboun,inpc,
     &  nline,ipoinp,inp,tieset,tietol,ntie,fmpc,cbody,ibody,xbody,
     &  nbody,nbody_,xbodyold,nam_,ielprop,nprop,nprop_,prop,itpamp,
     &  iviewfile,ipoinpc,cfd,nslavs,t0g,t1g,network,cyclicsymmetry,
     &  idefforc,idefload,idefbody)
!
      implicit none
!
!     nmethod: -1:visco (=static+creep) 0:no analysis; 1:static;
!               2:frequency 3:buckling; 4:linear dynamic
!     ithermal(1): 0:no thermal stresses; 1:thermal stresses;
!     iprestr: 0: no residual stresses; 1: residual stresses;
!     iperturb: 0:no perturbation; 1:perturbation; 2: nonlinear
!               geometric analysis; 3: material and geometrical
!               nonlinearities
!     istep: step number
!     istat: error indicator (<0:EOF, >0: input error)
!
      logical boun_flag,cload_flag,dload_flag,temp_flag,elprint_flag,
     &  nodeprint_flag,elfile_flag,nodefile_flag,contactfile_flag,
     &  dflux_flag,cflux_flag,film_flag,radiate_flag,out3d,
     &  solid,faceprint_flag,contactprint_flag
!
      character*1 typeboun(*),inpc(*)
      character*3 output
      character*87 filab(*)
      character*6 prlab(*)
      character*8 lakon(*)
      character*20 labmpc(*),sideload(*)
      character*80 matname(*),orname(*),amname(*)
      character*81 set(*),prset(*),tieset(3,*),cbody(*)
      character*132 jobnamec(*),textpart(16)
!
      integer kon(*),nodeboun(*),ndirboun(*),ipompc(*),nodempc(3,*),
     &  nodeforc(2,*),ndirforc(*),nelemload(2,*),iaxial,j,mi(*),
     &  istartset(*),iendset(*),ialset(*),ipkon(*),ics(*),
     &  nelcon(2,*),nrhcon(*),nalcon(2,*),ielmat(mi(3),*),
     &  ielorien(mi(3),*),icomposite,nsubmodel,mortar,
     &  namta(3,*),iamforc(*),iamload(2,*),iamt1(*),ipoinpc(0:*),
     &  iamboun(*),inotr(2,*),ikboun(*),ilboun(*),ikmpc(*),ilmpc(*),
     &  iponor(2,*),knor(*),ikforc(*),ilforc(*),iponoel(*),inoel(3,*),
     &  infree(4),ixfree,ikfree,inoelfree,iponoelmax,rig(*),nshcon(*),
     &  ncocon(2,*),nodebounold(*),ielprop(*),nprop,nprop_,maxsectors,
     &  ndirbounold(*),nnn(*),nline,ipoinp(2,*),inp(3,*),
     &  ianisoplas,cfd,ifile_output,ichangefriction,nslavs,
     &  nalset,nalset_,nmat,nmat_,ntmat_,norien,norien_,
     &  nmethod,nk,ne,nboun,nmpc,nmpc_,mpcfree,i,istat,n,
     &  key,nk_,ne_,nboun_,ncs_,namtot_,nstate_,iviewfile,
     &  isolver,ithermal(2),iperturb(*),iprestr,istep,mei(4),nkon,
     &  nprint,nload,nload_,nforc,nforc_,nlabel,iumat,imat,
     &  nset,nset_,nprint_,nam,nam_,jout(2),ncmat_,itpamp,
     &  ierror,idrct,jmax(2),iexpl,iplas,npmat_,ntrans,ntrans_,
     &  M_or_SPC,nplicon(0:ntmat_,*),nplkcon(0:ntmat_,*),nflow,
     &  ne1d,ne2d,nener,irstrt,ii,maxlenmpc,inl,ipol,network,
     &  iline,mcs,ntie,ntie_,lprev,newstep,nbody,nbody_,ibody(3,*),
     &  cyclicsymmetry,idefforc(*),idefload(*),idefbody(*)
!
      real*8 co(3,*),xboun(*),coefmpc(*),xforc(*),fmpc(*),
     &  xload(2,*),alzero(*),offset(2,*),prop(*),
     &  elcon(0:ncmat_,ntmat_,*),rhcon(0:1,ntmat_,*),
     &  alcon(0:6,ntmat_,*),thicke(mi(3),*),thickn(2,*),xnor(*),
     &  t1(*),orab(7,*),prestr(6,mi(1),*),amta(2,*),
     &  veold(0:mi(2),*),t0(*),plicon(0:2*npmat_,ntmat_,*),
     &  plkcon(0:2*npmat_,ntmat_,*),trab(7,*),dcs(*),
     &  shcon(0:3,ntmat_,*),cocon(0:6,ntmat_,*),
     &  ctrl(*),vold(0:mi(2),*),xbounold(*),xforcold(*),
     &  xloadold(*),t1old(*),eme(*),sti(*),ener(*),
     &  xstate(nstate_,mi(1),*),ttime,qaold(2),cs(17,*),tietol(2,*),
     &  xbody(7,*),xbodyold(7,*),t0g(2,*),t1g(2,*)
!
      real*8 fei(3),tinc,tper,xmodal(*),tmin,tmax,
     &  alpha,physcon(*)
!
      save iaxial,solid,ianisoplas,out3d
!
      integer nentries
      parameter(nentries=14)
!
      newstep=0
      iviewfile=0
      ichangefriction=0
      icomposite=0
!
      maxsectors=1
      if(mcs.ne.0) then
         do i=1,mcs
            maxsectors=max(maxsectors,int(cs(1,i)))
         enddo
      endif
!
      do i=1,nentries
         if(ipoinp(1,i).ne.0) then
            ipol=i
            inl=ipoinp(1,i)
            iline=inp(1,inl)-1
            exit
         endif
      enddo
!
      ixfree=infree(1)
      ikfree=infree(2)
      inoelfree=infree(3)
      iponoelmax=infree(4)
!
      iexpl=0
!
!     the following flag is used to check whether any SPC's or MPC's
!     are used before transformation definitions
!
      M_or_SPC=0
!
!     the flags indicate whether some specific keyword cards already
!     occurred (needed to determine the effect of OP=NEW or to check
!     whether the element or nodal output selection should be reset)
!
      boun_flag=.false.
      cload_flag=.false.
      dload_flag=.false.
      temp_flag=.false.
      elprint_flag=.false.
      nodeprint_flag=.false.
      faceprint_flag=.false.
      contactprint_flag=.false.
      contactfile_flag=.false.
      elfile_flag=.false.
      nodefile_flag=.false.
      film_flag=.false.
      dflux_flag=.false.
      radiate_flag=.false.
      cflux_flag=.false.
!
      nprint_=nprint
!
      nprint=0
!
      if(istep.eq.0) then
!
!        initializing the maxima
!
         ne_=ne
         nset_=nset
         nalset_=nalset
         nmat_=nmat
         norien_=norien
         ntrans_=ntrans
         ntie_=ntie
!
         nmethod=0
!
         ne=0
         nset=0
         nalset=0
         nmat=0
         norien=0
         ntrans=0
         ntie=0
         nsubmodel=0
!
         lprev=0
!
         do i=1,ne_
            ipkon(i)=-1
         enddo
!
         if((ne1d.gt.0).or.(ne2d.gt.0)) then
            do i=1,nlabel
               filab(i)='    I '
            enddo
            out3d=.false.
         else
            do i=1,nlabel
               filab(i)='      '
            enddo
            out3d=.true.
         endif
!
         iaxial=0
         solid=.false.
         ianisoplas=0
!
      endif
!
      call getnewline(inpc,textpart,istat,n,key,iline,ipol,inl,
     &    ipoinp,inp,ipoinpc)
!
      loop: do
!
         if(istat.lt.0) then
               write(*,*)
               write(*,*) 'Job finished'
               write(*,*)
               return
         endif
!
         if(textpart(1)(1:10).eq.'*AMPLITUDE') then
            call amplitudes(inpc,textpart,amname,amta,namta,nam,
     &        nam_,namtot_,irstrt,istep,istat,n,iline,ipol,inl,ipoinp,
     &        inp,ipoinpc)
!
         elseif(textpart(1)(1:12).eq.'*BEAMSECTION') then
            call beamsections(inpc,textpart,set,istartset,iendset,
     &           ialset,nset,ielmat,matname,nmat,ielorien,orname,norien,
     &           thicke,ipkon,iponor,xnor,ixfree,
     &           offset,lakon,irstrt,istep,istat,n,iline,ipol,inl,
     &           ipoinp,inp,ipoinpc,mi)
!
         elseif(textpart(1)(1:9).eq.'*BOUNDARY') then
            M_or_SPC=1
            call boundaries(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,nodeboun,ndirboun,xboun,nboun,nboun_,nk,
     &        iamboun,amname,nam,ipompc,nodempc,coefmpc,nmpc,nmpc_,
     &        mpcfree,inotr,trab,ntrans,ikboun,ilboun,ikmpc,ilmpc,
     &        nk_,co,labmpc,boun_flag,typeboun,istep,istat,n,iline,
     &        ipol,inl,ipoinp,inp,nam_,namtot_,namta,amta,nmethod,
     &        iperturb,iaxial,ipoinpc,vold,mi)
            boun_flag=.true.
!
         elseif(textpart(1)(1:7).eq.'*BUCKLE') then
            call buckles(inpc,textpart,nmethod,mei,fei,
     &        nforc,nload,ithermal,iprestr,nbody,t0,t1,nk,iperturb,
     &        istep,istat,n,iline,ipol,inl,ipoinp,inp,isolver,ipoinpc)
!
         elseif(textpart(1)(1:6).eq.'*CFLUX') then
            call cfluxes(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,nodeforc,ndirforc,xforc,nforc,nforc_,iamforc,
     &        amname,nam,ntrans,trab,inotr,co,ikforc,ilforc,nk,
     &        cflux_flag,istep,istat,n,iline,ipol,inl,ipoinp,inp,nam_,
     &        namtot_,namta,amta,iaxial,ipoinpc,idefforc)
            cflux_flag=.true.
!
         elseif(textpart(1)(1:15).eq.'*CHANGEFRICTION') then
            ichangefriction=1
            call changefrictions(inpc,textpart,matname,nmat,nmat_,
     &           irstrt,istep,istat,n,iline,ipol,inl,ipoinp,inp,
     &           ipoinpc,imat)
!
         elseif(textpart(1)(1:15).eq.'*CHANGEMATERIAL') then
            call changematerials(inpc,textpart,matname,nmat,nmat_,
     &           irstrt,istep,istat,n,iline,ipol,inl,ipoinp,inp,ipoinpc,
     &           imat)
!
         elseif(textpart(1)(1:14).eq.'*CHANGEPLASTIC') then
            call changeplastics(inpc,textpart,imat,ntmat_,npmat_,
     &        plicon,nplicon,plkcon,nplkcon,istep,istat,n,iline,ipol,
     &        inl,ipoinp,inp,ipoinpc,nelcon)
!
         elseif(textpart(1)(1:6).eq.'*CLOAD') then
            call cloads(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,nodeforc,ndirforc,xforc,nforc,nforc_,
     &        iamforc,amname,nam,ntrans,trab,inotr,co,ikforc,ilforc,
     &        nk,cload_flag,istep,istat,n,iline,ipol,inl,ipoinp,inp,
     &        nam_,namtot_,namta,amta,nmethod,iaxial,iperturb,ipoinpc,
     &        maxsectors,idefforc)
            cload_flag=.true.
!
         elseif(textpart(1)(1:17).eq.'*COMPLEXFREQUENCY') then
            call complexfrequencies(inpc,textpart,nmethod,
     &           mei,iperturb,istep,istat,n,iline,ipol,inl,
     &           ipoinp,inp,ithermal,xboun,nboun,ipoinpc,mcs,cs,
     &           cyclicsymmetry)
!
         elseif(textpart(1)(1:13).eq.'*CONDUCTIVITY') then
            call conductivities(inpc,textpart,cocon,ncocon,
     &           imat,ntmat_,irstrt,istep,istat,n,iline,ipol,inl,
     &           ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:15).eq.'*CONTACTDAMPING') then
            call contactdampings(inpc,textpart,elcon,nelcon,
     &           nmat,ntmat_,ncmat_,irstrt,istep,istat,n,iline,ipol,inl,
     &           ipoinp,inp,ipoinpc)
         elseif((textpart(1)(1:12).eq.'*CONTACTFILE').or.
     &          (textpart(1)(1:14).eq.'*CONTACTOUTPUT')) then
            if(textpart(1)(1:12).eq.'*CONTACTFILE') then
               output='asc'
            else
               output='bin'
            endif
            ifile_output=3
            call noelfiles(inpc,textpart,jout,filab,nmethod,
     &           nodefile_flag,elfile_flag,ifile_output,nener,ithermal,
     &           istep,istat,n,iline,ipol,inl,ipoinp,inp,out3d,nlabel,
     &           amname,nam,itpamp,idrct,ipoinpc,cfd,contactfile_flag,
     &           set,nset,xmodal)
            contactfile_flag=.true.
!
         elseif(textpart(1)(1:12).eq.'*CONTACTPAIR') then
            call contactpairs(inpc,textpart,tieset,cs,istep,
     &           istat,n,iline,ipol,inl,ipoinp,inp,ntie,ntie_,
     &           iperturb,matname,nmat,ipoinpc,tietol,set,nset,
     &           mortar)
!
         elseif(textpart(1)(1:13).eq.'*CONTACTPRINT') then
            call contactprints(inpc,textpart,nprint,nprint_,jout,
     &           prlab,prset,contactprint_flag,ithermal,istep,istat,n,
     &           iline,ipol,inl,ipoinp,inp,amname,nam,itpamp,idrct,
     &           ipoinpc,nener)
            contactprint_flag=.true.
!
         elseif(textpart(1)(1:9).eq.'*CONTROLS') then
            call controlss(inpc,textpart,ctrl,istep,istat,n,iline,
     &           ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:32).eq.'*COUPLEDTEMPERATURE-DISPLACEMENT')
     &           then
            call couptempdisps(inpc,textpart,nmethod,iperturb,isolver,
     &           istep,istat,n,tinc,tper,tmin,tmax,idrct,ithermal,iline,
     &           ipol,inl,ipoinp,inp,ipoinpc,alpha,ctrl,iexpl)
!
         elseif(textpart(1)(1:6).eq.'*CREEP') then
            call creeps(inpc,textpart,nelcon,imat,ntmat_,npmat_,
     &        plicon,nplicon,elcon,iplas,iperturb,nstate_,ncmat_,
     &        matname,irstrt,istep,istat,n,iline,ipol,inl,ipoinp,inp,
     &        ipoinpc,ianisoplas)
!
         elseif(textpart(1)(1:16).eq.'*CYCLICHARDENING') then
            call cychards(inpc,textpart,nelcon,imat,ntmat_,
     &        npmat_,plicon,nplicon,ncmat_,elcon,matname,
     &        irstrt,istep,istat,n,iline,ipol,inl,ipoinp,inp,ipoinpc)
!
c        ics(15*ncx_+1) was removed: can be cleaned up.
c
         elseif(textpart(1)(1:20).eq.'*CYCLICSYMMETRYMODEL') then
            call cycsymmods(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,tieset,tietol,co,nk,ipompc,nodempc,
     &        coefmpc,nmpc,nmpc_,ikmpc,ilmpc,mpcfree,dcs(lprev+1),
     &        dcs(ncs_+lprev+1),ics(lprev+1),ics(ncs_+lprev+1),
     &        ics(2*ncs_+lprev+1),dcs(2*ncs_+lprev+1),
     &        dcs(3*ncs_+lprev+1),ncs_,cs,labmpc,istep,istat,n,iline,
     &        ipol,inl,ipoinp,inp,ntie,mcs,lprev,ithermal,
     &        dcs(4*ncs_+1),dcs(6*ncs_+1),dcs(8*ncs_+1),dcs(10*ncs_+1),
     &        ics(3*ncs_+1),ics(5*ncs_+1),ics(7*ncs_+1),ics(8*ncs_+1),
     &        dcs(12*ncs_+1),ne,ipkon,kon,lakon,ics(14*ncs_+1),
     &        ics(16*ncs_+1),ics(18*ncs_+1),ipoinpc,
     &        maxsectors,trab,ntrans,ntrans_,jobnamec,vold,cfd,mi)
!
         elseif(textpart(1)(1:8).eq.'*DASHPOT') then
            call dashpots(inpc,textpart,nelcon,nmat,ntmat_,npmat_,
     &        plicon,nplicon,
     &        ncmat_,elcon,matname,irstrt,istep,istat,n,iline,ipol,
     &        inl,ipoinp,inp,nmat_,set,istartset,iendset,ialset,
     &        nset,ielmat,ielorien,ipoinpc,mi)
!
         elseif(textpart(1)(1:22).eq.'*DEFORMATIONPLASTICITY') then
            call defplasticities(inpc,textpart,elcon,nelcon,
     &        imat,ntmat_,ncmat_,irstrt,istep,istat,n,iperturb,
     &        iline,ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:8).eq.'*DENSITY') then
            call densities(inpc,textpart,rhcon,nrhcon,
     &        imat,ntmat_,irstrt,istep,istat,n,iline,ipol,
     &        inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:7).eq.'*DEPVAR') then
            call depvars(inpc,textpart,nelcon,imat,
     &           nstate_,irstrt,istep,istat,n,iline,ipol,inl,ipoinp,inp,
     &           ncocon,ipoinpc)
!
         elseif(textpart(1)(1:6).eq.'*DFLUX') then
            call dfluxes(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,nelemload,sideload,xload,nload,nload_,
     &        ielmat,ntmat_,iamload,amname,nam,lakon,ne,dflux_flag,
     &        istep,istat,n,iline,ipol,inl,ipoinp,inp,nam_,namtot_,
     &        namta,amta,ipoinpc,mi,idefload)
            dflux_flag=.true.
!
         elseif(textpart(1)(1:21).eq.'*DISTRIBUTINGCOUPLING') then
            call distributingcouplings(inpc,textpart,ipompc,nodempc,
     &           coefmpc,nmpc,nmpc_,mpcfree,nk,ikmpc,ilmpc,
     &           labmpc,istep,istat,n,iline,ipol,inl,ipoinp,inp,ipoinpc,
     &           lakon,kon,ipkon,set,nset,istartset,iendset,ialset,co)
!
         elseif((textpart(1)(1:6).eq.'*DLOAD').or.
     &          (textpart(1)(1:7).eq.'*DSLOAD')) then
            call dloads(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,nelemload,sideload,xload,nload,nload_,
     &        ielmat,iamload,
     &        amname,nam,lakon,ne,dload_flag,istep,istat,n,
     &        iline,ipol,inl,ipoinp,inp,cbody,ibody,xbody,nbody,nbody_,
     &        xbodyold,iperturb,physcon,nam_,namtot_,namta,amta,nmethod,
     &        ipoinpc,maxsectors,mi,idefload,idefbody)
            dload_flag=.true.
!
         elseif(textpart(1)(1:8).eq.'*DYNAMIC') then
            call dynamics(inpc,textpart,nmethod,iperturb,tinc,tper,
     &        tmin,tmax,idrct,alpha,iexpl,isolver,istep,
     &        istat,n,iline,ipol,inl,ipoinp,inp,ithermal,ipoinpc,cfd)
!
         elseif(textpart(1)(1:8).eq.'*ELASTIC') then
            call elastics(inpc,textpart,elcon,nelcon,
     &        imat,ntmat_,ncmat_,irstrt,istep,istat,n,
     &        iline,ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:17).eq.'*ELECTROMAGNETICS') then
            call electromagneticss(inpc,textpart,nmethod,iperturb,
     &           isolver,istep,istat,n,tinc,tper,tmin,tmax,idrct,
     &           ithermal,iline,ipol,inl,ipoinp,inp,alpha,mei,fei,
     &           ipoinpc,ctrl,ttime)
!
         elseif((textpart(1)(1:8).eq.'*ELEMENT').and.
     &          (textpart(1)(1:14).ne.'*ELEMENTOUTPUT')) then
            call elements(inpc,textpart,kon,ipkon,lakon,nkon,
     &        ne,ne_,set,istartset,iendset,ialset,nset,nset_,nalset,
     &        nalset_,mi(1),ixfree,iponor,xnor,istep,istat,n,iline,
     &        ipol,inl,ipoinp,inp,iaxial,ipoinpc,solid,cfd,
     &        network,filab,nlabel,out3d)
!
         elseif((textpart(1)(1:7).eq.'*ELFILE').or.
     &          (textpart(1)(1:14).eq.'*ELEMENTOUTPUT')) then
            if(textpart(1)(1:7).eq.'*ELFILE') then
               output='asc'
            else
               output='bin'
            endif
            ifile_output=2
            call noelfiles(inpc,textpart,jout,filab,nmethod,
     &           nodefile_flag,elfile_flag,ifile_output,nener,ithermal,
     &           istep,istat,n,iline,ipol,inl,ipoinp,inp,out3d,nlabel,
     &           amname,nam,itpamp,idrct,ipoinpc,cfd,contactfile_flag,
     &           set,nset,xmodal)
            elfile_flag=.true.
!
         elseif(textpart(1)(1:8).eq.'*ELPRINT') then
            call elprints(inpc,textpart,set,
     &        nset,nprint,nprint_,jout,
     &        prlab,prset,nmethod,elprint_flag,nener,ithermal,
     &        istep,istat,n,iline,ipol,inl,ipoinp,inp,amname,nam,itpamp,
     &        idrct,ipoinpc,cfd)
            elprint_flag=.true.
!
         elseif(textpart(1)(1:6).eq.'*ELSET') then
            call noelsets(inpc,textpart,set,istartset,iendset,ialset,
     &        nset,nset_,nalset,nalset_,nk,ne,irstrt,istep,istat,n,
     &        iline,ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:8).eq.'*ENDSTEP') then
            exit
!
         elseif(textpart(1)(1:9).eq.'*EQUATION') then
            M_or_SPC=1
            call equations(inpc,textpart,ipompc,nodempc,coefmpc,
     &        nmpc,nmpc_,mpcfree,nk,co,trab,inotr,ntrans,ikmpc,ilmpc,
     &        labmpc,istep,istat,n,iline,ipol,inl,ipoinp,inp,ipoinpc,
     &        set,istartset,iendset,ialset,nset)
!
         elseif(textpart(1)(1:10).eq.'*EXPANSION') then
            call expansions(inpc,textpart,alcon,nalcon,
     &        alzero,imat,ntmat_,irstrt,istep,istat,n,iline,
     &        ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:10).eq.'*FACEPRINT') then
            call faceprints(inpc,textpart,set,istartset,iendset,ialset,
     &        nset,nset_,nalset,nprint,nprint_,jout,
     &        prlab,prset,faceprint_flag,ithermal,istep,istat,n,iline,
     &        ipol,inl,ipoinp,inp,amname,nam,itpamp,idrct,ipoinpc,cfd)
            faceprint_flag=.true.
!
         elseif(textpart(1)(1:5).eq.'*FILM') then
            call films(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,nelemload,sideload,xload,nload,nload_,
     &        ielmat,ntmat_,iamload,amname,nam,lakon,ne,film_flag,
     &        istep,istat,n,iline,ipol,inl,ipoinp,inp,nam_,namtot_,
     &        namta,amta,ipoinpc,mi)
            film_flag=.true.
!
         elseif(textpart(1)(1:15).eq.'*FLUIDCONSTANTS') then
            call fluidconstants(inpc,textpart,shcon,nshcon,
     &           imat,ntmat_,irstrt,istep,istat,n,iline,ipol,
     &           inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:13).eq.'*FLUIDSECTION') then
            call fluidsections(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,ielmat,matname,nmat,
     &        irstrt,istep,istat,n,
     &        iline,ipol,inl,ipoinp,inp,lakon,ielprop,nprop,
     &        nprop_,prop,iaxial,ipoinpc,mi)
!
         elseif(textpart(1)(1:10).eq.'*FREQUENCY') then
            call frequencies(inpc,textpart,nmethod,
     &        mei,fei,iperturb,istep,istat,n,iline,ipol,
     &        inl,ipoinp,inp,ithermal,isolver,xboun,nboun,ipoinpc,
     &        ipompc,labmpc,fmpc,ikmpc,ilmpc,nmpc)
!
         elseif(textpart(1)(1:9).eq.'*FRICTION') then
            call frictions(inpc,textpart,elcon,nelcon,
     &           imat,ntmat_,ncmat_,irstrt,istep,istat,n,iline,ipol,inl,
     &           ipoinp,inp,ipoinpc,nstate_,ichangefriction,
     &           mortar)
!
         elseif(textpart(1)(1:5).eq.'*GAP ') then
            call gaps(inpc,textpart,set,istartset,iendset,
     &           ialset,nset,nset_,nalset,nalset_,ipompc,nodempc,
     &           coefmpc,labmpc,nmpc,nmpc_,mpcfree,ikmpc,ilmpc,lakon,
     &           ipkon,kon,nk,nk_,nodeboun,ndirboun,ikboun,ilboun,
     &           nboun,nboun_,iperturb,ne_,co,xboun,ctrl,typeboun,
     &           istep,istat,n,iline,ipol,inl,ipoinp,inp,iamboun,nam,
     &           inotr,trab,ntrans,nmethod,ipoinpc,mi)
!
         elseif(textpart(1)(1:15).eq.'*GAPCONDUCTANCE') then
            call gapconductances(inpc,textpart,nelcon,nmat,ntmat_,
     &        npmat_,plkcon,nplkcon,iperturb,irstrt,istep,istat,n,iline,
     &        ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:8).eq.'*HEADING') then
            call headings(inpc,textpart,istat,n,iline,ipol,inl,
     &        ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:13).eq.'*HEATTRANSFER') then
            call heattransfers(inpc,textpart,nmethod,iperturb,isolver,
     &           istep,istat,n,tinc,tper,tmin,tmax,idrct,ithermal,iline,
     &           ipol,inl,ipoinp,inp,alpha,mei,fei,ipoinpc,ctrl,ttime)
!
         elseif(textpart(1)(1:13).eq.'*HYPERELASTIC') then
            call hyperelastics(inpc,textpart,elcon,nelcon,
     &        imat,ntmat_,ncmat_,irstrt,istep,istat,n,iperturb,
     &        iline,ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:10).eq.'*HYPERFOAM') then
            call hyperfoams(inpc,textpart,elcon,nelcon,
     &        imat,ntmat_,ncmat_,irstrt,istep,istat,n,iperturb,iline,
     &        ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:18).eq.'*INITIALCONDITIONS') then
            call initialconditions(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,t0,t1,prestr,iprestr,ithermal,veold,inoelfree,
     &        nk_,mi(1),istep,istat,n,iline,ipol,inl,ipoinp,inp,lakon,
     &        kon,co,ne,ipkon,vold,ipoinpc,xstate,nstate_,nk,t0g,
     &        t1g)
!
         elseif(textpart(1)(1:9).eq.'*MATERIAL') then
            call materials(inpc,textpart,matname,nmat,nmat_,
     &        irstrt,istep,istat,n,iline,ipol,inl,ipoinp,inp,
     &        ipoinpc,imat)
!
         elseif(textpart(1)(1:13).eq.'*MODALDAMPING') then
            call modaldampings(inpc,textpart,nmethod,xmodal,istep,
     &        istat,n,iline,ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:13).eq.'*MODALDYNAMIC') then
            call modaldynamics(inpc,textpart,nmethod,tinc,tper,iexpl,
     &        istep,istat,n,iline,ipol,inl,ipoinp,inp,iperturb,
     &        isolver,cs,mcs,ipoinpc,idrct,ctrl,tmin,tmax,
     &        nforc,nload,nbody,iprestr,t0,t1,ithermal,nk,vold,veold,
     &        xmodal,set,nset,mi,cyclicsymmetry)
!
         elseif(textpart(1)(1:12).eq.'*MODELCHANGE') then
            call modelchanges(inpc,textpart,tieset,istat,n,iline,
     &           ipol,inl,ipoinp,inp,ntie,ipoinpc,istep)
!
         elseif(textpart(1)(1:4).eq.'*MPC') then
            call mpcs(inpc,textpart,set,istartset,iendset,
     &           ialset,nset,nset_,nalset,nalset_,ipompc,nodempc,
     &           coefmpc,labmpc,nmpc,nmpc_,mpcfree,ikmpc,ilmpc,lakon,
     &           ipkon,kon,nk,nk_,nodeboun,ndirboun,ikboun,ilboun,
     &           nboun,nboun_,iperturb,ne_,co,xboun,ctrl,typeboun,
     &           istep,istat,n,iline,ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:11).eq.'*NOANALYSIS') then
            call noanalysis(inpc,textpart,nmethod,iperturb,istep,
     &        istat,n,iline,ipol,inl,ipoinp,inp,ipoinpc,tper)
!
         elseif(textpart(1)(1:15).eq.'*NODALTHICKNESS') then
            call nodalthicknesses(inpc,textpart,set,istartset,iendset,
     &           ialset,nset,thickn,nk,istep,istat,n,iline,ipol,inl,
     &           ipoinp,inp,iaxial,ipoinpc)
!
         elseif((textpart(1)(1:5).eq.'*NODE').and.
     &          (textpart(1)(1:10).ne.'*NODEPRINT').and.
     &          (textpart(1)(1:11).ne.'*NODEOUTPUT').and.
     &          (textpart(1)(1:9).ne.'*NODEFILE')) then
            call nodes(inpc,textpart,co,nk,nk_,set,istartset,iendset,
     &        ialset,nset,nset_,nalset,nalset_,istep,istat,n,iline,
     &        ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif((textpart(1)(1:9).eq.'*NODEFILE').or.
     &          (textpart(1)(1:11).eq.'*NODEOUTPUT')) then
            if(textpart(1)(1:9).eq.'*NODEFILE') then
               output='asc'
            else
               output='bin'
            endif
            ifile_output=1
            call noelfiles(inpc,textpart,jout,filab,nmethod,
     &           nodefile_flag,elfile_flag,ifile_output,nener,ithermal,
     &           istep,istat,n,iline,ipol,inl,ipoinp,inp,out3d,nlabel,
     &           amname,nam,itpamp,idrct,ipoinpc,cfd,contactfile_flag,
     &           set,nset,xmodal)
            nodefile_flag=.true.
!
         elseif(textpart(1)(1:10).eq.'*NODEPRINT') then
            call nodeprints(inpc,textpart,set,istartset,iendset,ialset,
     &        nset,nset_,nalset,nprint,nprint_,jout,
     &        prlab,prset,nodeprint_flag,ithermal,istep,istat,n,iline,
     &        ipol,inl,ipoinp,inp,amname,nam,itpamp,idrct,ipoinpc,cfd)
            nodeprint_flag=.true.
!
         elseif(textpart(1)(1:7).eq.'*NORMAL') then
            call normals(inpc,textpart,iponor,xnor,ixfree,
     &           ipkon,kon,nk,nk_,ne,lakon,istep,istat,n,iline,ipol,
     &           inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:5).eq.'*NSET') then
            call noelsets(inpc,textpart,set,istartset,iendset,ialset,
     &        nset,nset_,nalset,nalset_,nk,ne,irstrt,istep,istat,n,
     &        iline,ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:12).eq.'*ORIENTATION') then
            call orientations(inpc,textpart,orname,orab,norien,
     &        norien_,istep,istat,n,iline,ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:18).eq.'*PHYSICALCONSTANTS') then
            call physicalconstants(inpc,textpart,physcon,
     &        istep,istat,n,iline,ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:8).eq.'*PLASTIC') then
            call plastics(inpc,textpart,nelcon,imat,ntmat_,npmat_,
     &        plicon,nplicon,plkcon,nplkcon,iplas,iperturb,nstate_,
     &        ncmat_,elcon,matname,irstrt,istep,istat,n,iline,ipol,
     &        inl,ipoinp,inp,ipoinpc,ianisoplas)
!
         elseif(textpart(1)(1:19).eq.'*PRE-TENSIONSECTION') then
            call pretensionsections(inpc,textpart,ipompc,nodempc,
     &           coefmpc,nmpc,nmpc_,mpcfree,nk,ikmpc,ilmpc,
     &           labmpc,istep,istat,n,iline,ipol,inl,ipoinp,inp,ipoinpc,
     &           lakon,kon,ipkon,set,nset,istartset,iendset,ialset,co,
     &           ics,dcs,t0,ithermal,ne)
!
         elseif(textpart(1)(1:8).eq.'*RADIATE') then
            call radiates(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,nelemload,sideload,xload,nload,nload_,
     &        ielmat,ntmat_,iamload,amname,nam,lakon,ne,radiate_flag,
     &        istep,istat,n,iline,ipol,inl,ipoinp,inp,physcon,nam_,
     &        namtot_,namta,amta,ipoinpc,mi)
            radiate_flag=.true.
!
         elseif(textpart(1)(1:8).eq.'*RESTART') then
            call restarts(istep,nset,nload,nforc, nboun,nk,ne,
     &           nmpc,nalset,nmat,ntmat_,npmat_,norien,nam,nprint,
     &           mi(1),ntrans,ncs_,namtot_,ncmat_,mpcfree,
     &           maxlenmpc,ne1d,
     &           ne2d,nflow,nlabel,iplas,nkon,ithermal,nmethod,
     &           iperturb,nstate_,nener,set,istartset,iendset,ialset,co,
     &           kon,ipkon,lakon,nodeboun,ndirboun,iamboun,xboun,ikboun,
     &           ilboun,ipompc,nodempc,coefmpc,labmpc,ikmpc,ilmpc,
     &           nodeforc,ndirforc,iamforc,xforc,ikforc,ilforc,
     &           nelemload,iamload,sideload,xload,
     &           elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,plicon,
     &           nplicon,plkcon,nplkcon,orname,orab,ielorien,trab,inotr,
     &           amname,amta,namta,t0,t1,iamt1,veold,ielmat,
     &           matname,prlab,prset,filab,vold,nodebounold,
     &           ndirbounold,xbounold,xforcold,xloadold,t1old,eme,
     &           iponor,xnor,knor,thickn,thicke,offset,iponoel,
     &           inoel,rig,shcon,nshcon,cocon,
     &           ncocon,ics,sti,ener,xstate,jobnamec,infree,nnn,
     &           irstrt,inpc,textpart,istat,n,key,prestr,iprestr,
     &           cbody,ibody,xbody,nbody,xbodyold,ttime,qaold,
     &           cs,mcs,output,physcon,ctrl,typeboun,iline,ipol,inl,
     &           ipoinp,inp,fmpc,tieset,ntie,tietol,ipoinpc,nslavs,
     &           t0g,t1g)
!
         elseif(textpart(1)(1:10).eq.'*RIGIDBODY') then
            call rigidbodies(inpc,textpart,set,istartset,iendset,
     &           ialset,nset,nset_,nalset,nalset_,ipompc,nodempc,
     &           coefmpc,labmpc,nmpc,nmpc_,mpcfree,ikmpc,ilmpc,lakon,
     &           ipkon,kon,nk,nk_,nodeboun,ndirboun,ikboun,ilboun,
     &           nboun,nboun_,iperturb,ne_,ctrl,typeboun,
     &           istep,istat,n,iline,ipol,inl,ipoinp,inp,co,ipoinpc)
!
         elseif(textpart(1)(1:26).eq.'*SELECTCYCLICSYMMETRYMODES') then
            call selcycsymmods(inpc,textpart,cs,ics,tieset,istartset,
     &        iendset,ialset,ipompc,nodempc,coefmpc,nmpc,nmpc_,ikmpc,
     &        ilmpc,mpcfree,mcs,set,nset,labmpc,istep,istat,n,iline,
     &        ipol,inl,ipoinp,inp,nmethod,key,ipoinpc)
!
         elseif(textpart(1)(1:13).eq.'*SHELLSECTION') then
            call shellsections(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,ielmat,matname,nmat,ielorien,orname,
     &        norien,thicke,kon,ipkon,offset,irstrt,istep,istat,n,
     &        iline,ipol,inl,ipoinp,inp,lakon,iaxial,ipoinpc,mi,
     &        icomposite)
!
         elseif(textpart(1)(1:13).eq.'*SOLIDSECTION') then
            call solidsections(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,ielmat,matname,nmat,ielorien,orname,
     &        norien,lakon,thicke,kon,ipkon,irstrt,istep,istat,n,iline,
     &        ipol,inl,ipoinp,inp,cs,mcs,iaxial,ipoinpc,mi)
!
         elseif(textpart(1)(1:20).eq.'*SPECIFICGASCONSTANT') then
            call specificgasconstants(inpc,textpart,shcon,nshcon,
     &           imat,ntmat_,irstrt,istep,istat,n,iline,ipol,
     &           inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:13).eq.'*SPECIFICHEAT') then
            call specificheats(inpc,textpart,shcon,nshcon,
     &           imat,ntmat_,irstrt,istep,istat,n,iline,ipol,
     &           inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:7).eq.'*SPRING') then
            call springs(inpc,textpart,nelcon,nmat,ntmat_,npmat_,
     &        plicon,nplicon,
     &        ncmat_,elcon,matname,irstrt,istep,istat,n,iline,ipol,
     &        inl,ipoinp,inp,nmat_,set,istartset,iendset,ialset,
     &        nset,ielmat,ielorien,ipoinpc,mi)
!
         elseif(textpart(1)(1:7).eq.'*STATIC') then
            call statics(inpc,textpart,nmethod,iperturb,isolver,istep,
     &        istat,n,tinc,tper,tmin,tmax,idrct,iline,ipol,inl,ipoinp,
     &        inp,ithermal,cs,ics,tieset,istartset,
     &        iendset,ialset,ipompc,nodempc,coefmpc,nmpc,nmpc_,ikmpc,
     &        ilmpc,mpcfree,mcs,set,nset,labmpc,ipoinpc,iexpl,cfd,ttime,
     &        iaxial,nelcon,nmat)
!
         elseif(textpart(1)(1:20).eq.'*STEADYSTATEDYNAMICS') then
            call steadystatedynamics(inpc,textpart,nmethod,
     &        iexpl,istep,istat,n,iline,ipol,inl,ipoinp,inp,iperturb,
     &        isolver,xmodal,cs,mcs,ipoinpc,nforc,nload,nbody,iprestr,
     &        t0,t1,ithermal,nk,set,nset,cyclicsymmetry)
!
         elseif(textpart(1)(1:5).eq.'*STEP') then
            call steps(inpc,textpart,iperturb,iprestr,nbody,nforc,
     &                 nload,ithermal,t0,t1,nk,irstrt,istep,istat,n,
     &                 jmax,ctrl,iline,ipol,inl,ipoinp,inp,newstep,
     &                 ipoinpc,physcon)
!
         elseif(textpart(1)(1:9).eq.'*SUBMODEL') then
            call submodels(inpc,textpart,set,istartset,iendset,ialset,
     &           nset,nset_,nalset,nalset_,nk,istep,istat,n,iline,ipol,
     &           inl,ipoinp,inp,ipoinpc,nsubmodel,tieset,tietol,ntie,
     &           ntie_,jobnamec,amta,namta,nam,nam_,namtot_)
!
         elseif(textpart(1)(1:9).eq.'*SURFACE ') then
            call surfaces(inpc,textpart,set,istartset,iendset,ialset,
     &           nset,nset_,nalset,nalset_,nk,ne,istep,istat,n,iline,
     &           ipol,inl,ipoinp,inp,lakon,ipoinpc)
!
         elseif(textpart(1)(1:16).eq.'*SURFACEBEHAVIOR') then
            call surfacebehaviors(inpc,textpart,elcon,nelcon,
     &           nmat,ntmat_,ncmat_,irstrt,istep,istat,n,iline,ipol,inl,
     &           ipoinp,inp,ipoinpc,npmat_,plicon,nplicon)
!
         elseif(textpart(1)(1:19).eq.'*SURFACEINTERACTION') then
            call surfaceinteractions(inpc,textpart,matname,nmat,nmat_,
     &           irstrt,istep,istat,n,iline,ipol,inl,ipoinp,inp,nrhcon,
     &           ipoinpc,imat)
!
         elseif(textpart(1)(1:12).eq.'*TEMPERATURE') then
            call temperatures(inpc,textpart,set,istartset,iendset,
     &        ialset,nset,t0,t1,nk,ithermal,iamt1,amname,nam,
     &        inoelfree,nk_,nmethod,temp_flag,istep,istat,n,iline,
     &        ipol,inl,ipoinp,inp,nam_,namtot_,namta,amta,ipoinpc,t1g)
            temp_flag=.true.
!
         elseif(textpart(1)(1:4).eq.'*TIE') then
            call ties(inpc,textpart,tieset,tietol,istep,
     &           istat,n,iline,ipol,inl,ipoinp,inp,ntie,ntie_,ipoinpc)
!
         elseif(textpart(1)(1:11).eq.'*TIMEPOINTS') then
            call timepointss(inpc,textpart,amname,amta,namta,nam,
     &        nam_,namtot_,irstrt,istep,istat,n,iline,ipol,inl,ipoinp,
     &        inp,ipoinpc)
!
         elseif(textpart(1)(1:10).eq.'*TRANSFORM') then
            if(M_or_SPC.eq.1) then
               write(*,*) '*WARNING in calinput: SPCs or MPCs have'
               write(*,*) '         been defined before the definition'
               write(*,*) '         of a transformation'
            endif
            call transforms(inpc,textpart,trab,ntrans,ntrans_,
     &        inotr,set,istartset,iendset,ialset,nset,istep,istat,
     &        n,iline,ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:34).eq.
     &         '*UNCOUPLEDTEMPERATURE-DISPLACEMENT') then
            call uncouptempdisps(inpc,textpart,nmethod,iperturb,isolver,
     &           istep,istat,n,tinc,tper,tmin,tmax,idrct,ithermal,iline,
     &           ipol,inl,ipoinp,inp,ipoinpc,alpha,ctrl)
!
         elseif(textpart(1)(1:13).eq.'*USERMATERIAL') then
            call usermaterials(inpc,textpart,elcon,nelcon,
     &        imat,ntmat_,ncmat_,iperturb,iumat,irstrt,istep,istat,n,
     &        iline,ipol,inl,ipoinp,inp,cocon,ncocon,ipoinpc)
!
         elseif(textpart(1)(1:17).eq.'*VALUESATINFINITY') then
            call valuesatinf(inpc,textpart,physcon,
     &        istep,istat,n,iline,ipol,inl,ipoinp,inp,ipoinpc)
!
         elseif(textpart(1)(1:11).eq.'*VIEWFACTOR') then
            call viewfactors(textpart,iviewfile,istep,inpc,
     &           istat,n,key,iline,ipol,inl,ipoinp,inp,jobnamec,ipoinpc)
!
         elseif(textpart(1)(1:7).eq.'*VISCO') then
            call viscos(inpc,textpart,nmethod,iperturb,isolver,istep,
     &        istat,n,tinc,tper,tmin,tmax,idrct,iline,ipol,inl,ipoinp,
     &        inp,ipoinpc,nelcon,nmat,ncmat_)
!
!           check for zero-character-lines?
!
         elseif(ipoinpc(iline-1).eq.ipoinpc(iline)) then
            call getnewline(inpc,textpart,istat,n,key,iline,ipol,inl,
     &         ipoinp,inp,ipoinpc)
!     
         else
            write(*,*) '*WARNING in calinput. Card image cannot be inter
     &preted:'
            call inputwarning(inpc,ipoinpc,iline)
            call getnewline(inpc,textpart,istat,n,key,iline,ipol,inl,
     &         ipoinp,inp,ipoinpc)
         endif
!           
      enddo loop
!
!     check whether the *END STEP card was preceded by a *STEP card    
!
      if(newstep.eq.0) then
         write(*,*) '*ERROR in calinput: *END STEP card in step ',
     &           istep+1
         write(*,*) '       was not preceded by a *STEP card'
      endif
!
!     reorganizing the input in field inpc
!
      j=1
      do
         if(j.eq.1) then
            inp(1,j)=iline+1
         else
c            inp(1,j)=inp(1,inl)-iline
            inp(1,j)=inp(1,inl)
         endif
c         inp(2,j)=inp(2,inl)-iline
         inp(2,j)=inp(2,inl)
         if(inp(3,inl).eq.0) then
            inp(3,j)=0
            ipoinp(2,nentries)=j
            exit
         else
            inl=inp(3,inl)
            inp(3,j)=j+1
            j=j+1
         endif
      enddo
      do j=1,nentries-1
         ipoinp(1,j)=0
      enddo
      ipoinp(1,nentries)=1
c      do j=iline+1,nline
c         inpc(j-iline)=inpc(j)
c      enddo
c      nline=nline-iline
c      call writeinput(inpc,ipoinp,inp,nline,ipoinp(2,12))
!
!     expanding the 1-D and 2-D elements to volume elements
!     treating the incompressibility constraint
!
      do j=1,nforc_
         idefforc(j)=0
      enddo
!
      call gen3delem(kon,ipkon,lakon,ne,ipompc,nodempc,coefmpc,
     &  nmpc,nmpc_,mpcfree,ikmpc,ilmpc,labmpc,ikboun,ilboun,nboun,
     &  nboun_,nodeboun,ndirboun,xboun,iamboun,nam,
     &  inotr,trab,nk,nk_,iponoel,inoel,iponor,xnor,thicke,thickn,
     &  knor,istep,offset,t0,t1,ikforc,ilforc,rig,nforc,
     &  nforc_,nodeforc,ndirforc,xforc,iamforc,nelemload,sideload,
     &  nload,ithermal,ntrans,co,ixfree,ikfree,inoelfree,iponoelmax,
     &  iperturb,tinc,tper,tmin,tmax,ctrl,typeboun,nmethod,nset,set,
     &  istartset,iendset,ialset,prop,ielprop,vold,mi,nkon,ielmat,
     &  icomposite,t0g,t1g,idefforc)
!
!     New multistage Routine Call
!
      call multistages(nkon,set,istartset,iendset,
     &        ialset,nset,tieset,tietol,co,nk,ipompc,nodempc,
     &        coefmpc,nmpc,nmpc_,ikmpc,ilmpc,mpcfree,dcs(lprev+1),
     &        dcs(ncs_+lprev+1),ics(lprev+1),ics(ncs_+lprev+1),
     &        ics(2*ncs_+lprev+1),dcs(2*ncs_+lprev+1),
     &        dcs(3*ncs_+lprev+1),ncs_,cs,labmpc,ntie,mcs,
     &        dcs(4*ncs_+1),dcs(6*ncs_+1),dcs(8*ncs_+1),dcs(10*ncs_+1),
     &        ics(3*ncs_+1),ics(5*ncs_+1),ics(7*ncs_+1),ics(8*ncs_+1),
     &        dcs(12*ncs_+1),ne,ipkon,kon,lakon,ics(14*ncs_+1),
     &        ics(16*ncs_+1),ics(18*ncs_+1))
!
      infree(1)=ixfree
      infree(2)=ikfree
      infree(3)=inoelfree
      infree(4)=iponoelmax
!
!     check of the selected options
!
      if((filab(11)(1:2).eq.'PU').or.(filab(18)(1:3).eq.'PHS').or.
     &   (filab(19)(1:4).eq.'MAXU').or.(filab(20)(1:4).eq.'MAXS')) then
         if((nmethod.eq.1).or.(nmethod.eq.3).or.(nmethod.eq.4).or.
     &      ((nmethod.eq.2).and.((mcs.eq.0).or.(cs(2,1).lt.0)))) then
            write(*,*) '*WARNING in calinput: PU, PHS, MAXU or MAXS'
            write(*,*) '         was selected for a static, a non-'
            write(*,*) '         cyclic-symmetric frequency, a'
            write(*,*) '         buckling or a modal dynamic'
            write(*,*) '         calculation; the output option is'
            write(*,*) '         removed.'
            filab(11)(1:4)='    '
            filab(18)(1:4)='    '
            filab(19)(1:4)='    '
            filab(20)(1:4)='    '
            write(*,*) nmethod,mcs,cs(2,1)
         endif
      endif
!
      if(((iplas.eq.0).and.(ianisoplas.eq.0)).or.(nmethod.eq.2)) then
         if(filab(6)(1:4).eq.'PEEQ') then
            write(*,*) '*WARNING in calinput: PEEQ-output requested'
            write(*,*) '         yet no (visco)plastic calculation'
            filab(6)='      '
         endif
         ii=0
         do i=1,nprint
            if(prlab(i)(1:4).eq.'PEEQ') then
               write(*,*) '*WARNING in calinput: PEEQ-output requested'
               write(*,*) '         yet no (visco)plastic calculation'
               cycle
            endif
            ii=ii+1
            prlab(ii)=prlab(i)
            prset(ii)=prset(i)
         enddo
         nprint=ii
      endif
!
      if(ithermal(1).eq.0) then
         if(filab(2)(1:2).eq.'NT') then
            write(*,*) '*WARNING in calinput: temperature output'
            write(*,*) '         requested, yet no thermal loading'
            write(*,*) '         active'
            filab(2)='      '
         endif
         ii=0
         do i=1,nprint
            if(prlab(i)(1:4).eq.'NT  ') then
               write(*,*) '*WARNING in calinput: temperature output'
               write(*,*) '         requested, yet no thermal loading'
               write(*,*) '         active'
               cycle
            endif
            ii=ii+1
            prlab(ii)=prlab(i)
            prset(ii)=prset(i)
         enddo
         nprint=ii
      endif
!
      if(ithermal(1).le.1) then
         if(filab(9)(1:3).eq.'HFL') then
            write(*,*) '*WARNING in calinput: heat flux output'
            write(*,*) '         requested, yet no heat transfer'
            write(*,*) '         calculation'
            filab(9)='      '
         endif
         if(filab(10)(1:3).eq.'RFL') then
            write(*,*) '*WARNING in calinput: heat source output'
            write(*,*) '         requested, yet no heat transfer'
            write(*,*) '         calculation'
            filab(10)='      '
         endif
         if(filab(14)(1:2).eq.'MF') then
            write(*,*) '*WARNING in calinput: mass flow output'
            write(*,*) '         requested, yet no heat transfer'
            write(*,*) '         calculation'
            filab(14)='      '
         endif
         if(filab(15)(1:2).eq.'PT') then
            write(*,*) '*WARNING in calinput: total pressure output'
            write(*,*) '         requested, yet no heat transfer'
            write(*,*) '         calculation'
            filab(15)='      '
         endif
         if(filab(16)(1:2).eq.'TT') then
            write(*,*) '*WARNING in calinput: total temperature output'
            write(*,*) '         requested, yet no heat transfer'
            write(*,*) '         calculation'
            filab(16)='      '
         endif
      endif
!
!     check whether a material was assigned to each active element
!
      ierror=0
      do i=1,ne
         if(ipkon(i).lt.0) cycle
!
!        gaps and DCOUP3D-elements do not need a material assignment
!
         if(lakon(i)(1:1).eq.'G') cycle
         if(lakon(i)(1:7).eq.'DCOUP3D') cycle
         if(ielmat(1,i).eq.0) then
            ierror=1
            write(*,*) '*ERROR in calinput: no material was assigned'
            write(*,*) '       to element ',i
         endif
      enddo
      if(ierror.eq.1) stop
!
!     check whether for mechanical calculations with temperature
!     conditions an initial and final temperature value was assigned
!     to each node belonging to an element
!
      if((ithermal(1).eq.1).and.(istep.eq.1)) then
         call checktemp(t0,t1,lakon,ne,ipkon,kon)
      endif
!
!     check whether the density was defined for dynamic calculations
!     and transient thermal calculations
!
      if(((nbody.gt.0).or.
     &   (nmethod.eq.2).or.(nmethod.eq.4)).and.(cfd.eq.0)) then
         ierror=0
         do i=1,nmat
            if((nrhcon(i).ne.0).or.(matname(i)(1:6).eq.'SPRING').or.
     &         (matname(i)(1:7).eq.'DASHPOT')) then
               ierror=ierror+1
            else
               write(*,*)'*WARNING in calinput: no density was assigned'
               write(*,*) '         to material ',
     &              matname(i)(1:index(matname(i),' ')-1),
     &              ' in a dynamic'
               write(*,*) '         calculation or a calculation with'
               write(*,*) '         centrifugal or gravitational loads'
            endif
         enddo
         if(ierror.eq.0) then
            write(*,*) '*ERROR in calinput: no density was assigned'
            write(*,*) '       to any material ',
     &           ' in a dynamic'
            write(*,*) '       calculation or a calculation with'
            write(*,*) '       centrifugal or gravitational loads'
            stop
         endif
      endif
!
!     check whether the specific heat was defined for 
!     transient thermal calculations
!
      if((nmethod.eq.2).or.(nmethod.eq.4)) then
         if(ithermal(1).ge.2) then
            ierror=0
            do i=1,nmat
               if(nshcon(i).ne.0) then
                  ierror=ierror+1
               else
                  write(*,*) '*WARNING in calinput: no specific heat '
                  write(*,*) '         was assigned to material ',
     &                 matname(i)(1:index(matname(i),' ')-1),
     &                 ' in a transient'
                  write(*,*) '         heat transfer calculation'
                  write(*,*)
               endif
            enddo
            if(ierror.eq.0) then
               write(*,*) '*ERROR in calinput: no specific heat was'
               write(*,*) '       assigned to any material ',
     &              ' in a transient'
               write(*,*) '       heat transfer calculation'
               stop
            endif
         endif
      endif
!
!     check whether a *FLUID CONSTANTS card was used for 
!     3D compressible fluid calculations
!
      if((cfd.eq.1).or.(network.eq.1)) then
         ierror=0
         do i=1,nmat
            if(nshcon(i).ne.0) then
               ierror=ierror+1
            else
               write(*,*) '*WARNING in calinput: no specific heat '
               write(*,*) '         was assigned to material ',
     &              matname(i)(1:index(matname(i),' ')-1),
     &              ' in a transient'
               write(*,*) '         heat transfer calculation'
               write(*,*)
            endif
         enddo
         if(ierror.eq.0) then
            write(*,*) '*ERROR in calinput: no specific heat was'
            write(*,*) '       assigned to any material ',
     &           ' in a transient'
            write(*,*) '       heat transfer calculation'
            stop
         endif
      endif
!
!     check whether the elastic constants were defined for 
!     mechanical calculations
!
      if((ithermal(1).ne.2).and.solid) then
         ierror=0
         do i=1,nmat
            if(nelcon(1,i).ne.0) then
               ierror=ierror+1
            else
               write(*,*)'*WARNING in calinput: no elastic constants '
               write(*,*)' were assigned to material ',
     &              matname(i)(1:index(matname(i),' ')-1)
               write(*,*) '         in a (thermo)mechanical calculation'
               write(*,*)
            endif
         enddo
         if(ierror.eq.0) then
            write(*,*) '*ERROR in calinput: no elastic constants'
            write(*,*) '       were assigned to any material in a'
            write(*,*) '       (thermo)mechanical calculation'
            stop
         endif
      endif
!
!     check whether the conductivity was defined for thermal calculations
!
      if((ithermal(1).ge.2).and.(cfd.eq.0)) then
         ierror=0
         do i=1,nmat
            if(ncocon(1,i).ne.0) then
               ierror=ierror+1
            else
               write(*,*) '*WARNING in calinput: no conductivity '
               write(*,*) 
     &              '         constants were assigned to material ',
     &                      matname(i)(1:index(matname(i),' ')-1)
               write(*,*) '         in a thermo(mechanical) calculation'
               write(*,*)
            endif
         enddo
      endif
!
      if(cfd.eq.1) then
         if(iperturb(1).eq.0) then
            iperturb(1)=2
         elseif(iperturb(1).eq.1) then
            write(*,*) '*ERROR in calinput: PERTURBATION and fluids'
            write(*,*) '       are mutually exclusive; '
            call inputerror(inpc,ipoinpc,iline)
            stop
         endif
      endif
!
      write(*,*)
      write(*,*) 'STEP ',istep
      write(*,*)
c      write(5,*)
c      write(5,*) 'STEP ',istep
c      write(5,*)
      if(nmethod.eq.-1) then
         write(*,*) 'Visco analysis was selected'
      elseif(nmethod.eq.0) then
         write(*,*) 'No analysis was selected'
      elseif(nmethod.eq.1) then
         write(*,*) 'Static analysis was selected'
      elseif(nmethod.eq.2) then
         write(*,*) 'Frequency analysis was selected'
      elseif(nmethod.eq.3) then
         write(*,*) 'Buckling analysis was selected'
      elseif(nmethod.eq.4) then
         write(*,*) 'Dynamic analysis was selected'
      endif
      write(*,*)
      if(iperturb(1).eq.1) then
         write(*,*) 'Perturbation parameter is active'
         write(*,*)
c      elseif(iperturb(1).eq.2) then
c         write(*,*) 'Newton-Raphson iterative procedure is activated'
c         write(*,*)
      elseif(iperturb(1).eq.3) then
         write(*,*) 'Nonlinear material laws are taken into account'
         write(*,*)
      endif
!
      if(iperturb(1).ge.2) then
         write(*,*) 'Newton-Raphson iterative procedure is active'
         write(*,*)
      endif
!
      if(iperturb(2).eq.1) then
         write(*,*) 'Nonlinear geometric effects are taken into account'
         write(*,*)
      endif
!
      return
      end
