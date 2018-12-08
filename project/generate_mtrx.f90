program generate_mtrx
  implicit none

  integer :: nglobal
  integer :: i, j, k
  real(kind = 8), ALLOCATABLE, DIMENSION(:,:) :: mtrxA
  real(kind = 8), ALLOCATABLE, DIMENSION(:) :: mtrxB
  CHARACTER(LEN=6) :: flna, flnb
  CHARACTER(LEN=4) :: fs
flna = "mtrxa_"
flnb = "mtrxb_"

do k = 12,12
  nglobal = 2**k
  ALLOCATE(mtrxA(nglobal,nglobal),mtrxB(nglobal))
  do i = 1, nglobal
    do j = 1, nglobal
     mtrxA(i,j) = rand()
    enddo
    mtrxB(i) = rand()
  enddo

  WRITE(fs,"(I4)") 1000+nglobal

call OutPut1D_F(flnb//fs, nglobal, 1, mtrxB)
call OutPut2D_F(flna//fs, nglobal, 1, mtrxA)
DEALLOCATE(mtrxA, mtrxB)
enddo

end program generate_mtrx

SUBROUTINE OutPut1D_F(prfn, px, AB, mtrxout)
  IMPLICIT NONE

  INTEGER,INTENT(IN) :: px
  REAL(KIND=8),INTENT(IN),DIMENSION(px) :: mtrxout
  INTEGER,INTENT(IN) :: AB
  CHARACTER(LEN=10),INTENT(IN) :: prfn
  CHARACTER(LEN=14) :: flnm

  !WRITE(fs,"(I4)") 1000+fr
  !print*, fr
  !! case 1 ==> ascii; case 2 ==> binary
  SELECT CASE (AB)
  	CASE (1)
  		flnm = prfn//'.txt'
  		OPEN(UNIT=20,FILE=flnm,FORM='FORMATTED',STATUS='REPLACE',ACTION='WRITE')
  			WRITE(20,*) mtrxout(1:px)
  		CLOSE(20)
  	CASE (2)
  		flnm = prfn//'.dat'
  		OPEN(UNIT=20,FILE=flnm,FORM='UNFORMATTED',STATUS='REPLACE',ACTION='WRITE')
  			WRITE(20) mtrxout(1:px)
  		CLOSE(20)
  END SELECT


END SUBROUTINE OutPut1D_F

SUBROUTINE OutPut2D_F(prfn, px, AB, mtrxout)
  IMPLICIT NONE

  INTEGER,INTENT(IN) :: px
  REAL(KIND=8),INTENT(IN),DIMENSION(px,px) :: mtrxout
  INTEGER,INTENT(IN) :: AB
  CHARACTER(LEN=10),INTENT(IN) :: prfn
  CHARACTER(LEN=14) :: flnm

  !WRITE(fs,"(I4)") 1000+fr
  !print*, fr
  !! case 1 ==> ascii; case 2 ==> binary
  SELECT CASE (AB)
  	CASE (1)
  		flnm = prfn//'.txt'
  		OPEN(UNIT=20,FILE=flnm,FORM='FORMATTED',STATUS='REPLACE',ACTION='WRITE')
  			WRITE(20,*) mtrxout(1:px,1:px)
  		CLOSE(20)
  	CASE (2)
  		flnm = prfn//'.dat'
  		OPEN(UNIT=20,FILE=flnm,FORM='UNFORMATTED',STATUS='REPLACE',ACTION='WRITE')
  			WRITE(20) mtrxout(1:px,1:px)
  		CLOSE(20)
  END SELECT


END SUBROUTINE OutPut2D_F
