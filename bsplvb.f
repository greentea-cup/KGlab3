      SUBROUTINE INTERV ( XT, LXT, X, LEFT, MFLAG )
      INTEGER LEFT,LXT,MFLAG,   IHI,ILO,ISTEP,MIDDLE
      REAL X,XT(LXT)
      DATA ILO /1/
      SAVE ILO
      IHI = ILO + 1
      IF (IHI .LT. LXT)                GO TO 20
         IF (X .GE. XT(LXT))           GO TO 110
         IF (LXT .LE. 1)               GO TO 90
         ILO = LXT - 1
         IHI = LXT


      SUBROUTINE BSPLVB ( T, LENT, JHIGH, INDEX, X, LEFT, BIATX )
C FROM * A PRACTICAL GUILDE TO SPLINES * BY C. DE BOOR
C REST COMMENTS WERE NOT COPIED
      INTEGER INDEX,JHIGH,LEFT,    I,J,JMAX,JP1, LENT
      PARAMETER (JMAX = 20)
      REAL BIATX(JHIGH),T(LENT),X, DELTAL(JMAX),DELTAR(JMAX),SAVED,TERM
C     DIMENSION BIATX(JOUT), T(LEFT+JOUT)
      DATA J/1/
      SAVE J,DELTAL,DELTAR
C
                                        GO TO (10,20), INDEX
   10 J = 1
      BIATX(1) = 1.
      IF (J .GE. JHIGH)                 GO TO 99
C
   20    JP1 = J + 1
         DELTAR(J) = T(LEFT+J) - X
         DELTAL(J) = X - T(LEFT+1-J)
         SAVED = 0.
         DO 26 I=1,J
            TERM = BIATX(I)/(DELTAR(I) + DELTAL(JP1-I))
            BIATX(I) = SAVED + DELTAR(I)*TERM
   26       SAVED = DELTAL(JP1-I)*TERM
         BIATX(JPl) = SAVED
         J = JP1
         IF (J .LT. JHIGH)              GO TO 20
C
   99                                   RETURN
      END

CHAPTER X. EXAMPLE 1. PLOTTING SOME B-SPLINES
CALLS  BSPLVB, INTERV
      INTEGER I,J,K,LEFT,LEFTMK,MFLAG,N,NPOINT
      REAL DX,T(10),VALUES(7),X,XL
C DIMENSION, ORDER AND KNOT SEQUENCE FOR SPLINE SPACE ARE SPECIFIED...
      DATA N,K /7,3/, T /3*0.,2*1.,3.,4.,3*6./
C B-SPLINE VALUES ARE INITIALIZED TO 0., NUMBER OF EVALUATION POINTS...
      DATA VALUES /7*0./, NPOINT /31/
C SET LEFTMOST EVALUATION POINT XL AND SPACING DX TO BE USED...
      XL = T(K)
      DX = (T(N+1)-T(K))/FLOAT(NPOINT-1)
C
      PRINT 600,(I,I=1,5)
  600 FORMAT('1  X',8X,5('B',I1,'(X)',7X))
C
      DO 10 I=1,NPOINT
         X = XL + FLOAT(I-1)*DX
C                     LOCATE X WITH RESPECT TO KNOT ARRAY T .
         CALL INTERV ( T, N+1, X, LEFT, MFLAG )
         LEFTMK = LEFT - K
C           GET B(I,K)(X) IN VALUES(I) , I=1,...,N . К OF THESE,
C        VIZ.  B(LEFT-K+1,K)(X), ..., B(LEFT,K)(X), ARE SUPPLIED BY
C        BSPLVB . ALL OTHERS ARE KNOWN TO BE ZERO A PRIORI.
         CALL BSPLVB ( T, 10, K, 1, X, LEFT, VALUES(LEFTMK+1) )
C
         PRINT 610, X, (VALUES(J),J=3,7)
  610    FORMAT(F7.3,5F12.7)
C
C           ZERO OUT THE VALUES JUST COMPUTED IN PREPARATION FOR NEXT
C        EVALUATION POINT .
         DO 10 J=1,K
   10       VALUES(LEFTMK+J) = 0.
                                        STOP
      END


