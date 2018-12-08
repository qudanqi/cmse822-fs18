PROGRAM Gauss_Elimination_v02
use omp_lib
IMPLICIT NONE
include "mpif.h"

integer :: ierr, myRank, numRanks
INTEGER, PARAMETER :: nglobal = 4
INTEGER :: i, j, k, iter
REAL(KIND=8), DIMENSION(nglobal,nglobal) :: mtrxA, mtrxA1
REAL(KIND=8), DIMENSION(nglobal) :: mtrxB, mtrxB1, mtrxX, mtrxX1
REAL(KIND=8), ALLOCATABLE, DIMENSION(:,:) :: mtrx_A
REAL(KIND=8), ALLOCATABLE, DIMENSION(:) :: mtrx_B, mtrx_X, tempA, tempB
INTEGER :: nlocal
REAL(KIND=8) :: time1, time2, timef, diff
CHARACTER(LEN=6) :: flna, flnb, flnx
CHARACTER(LEN=4) :: fs
flna = "mtrxa_"
flnb = "mtrxb_"
flnx = "mtrxx_"
! write the number after file name
WRITE(fs,"(I4)") 1000 + nglobal
! inputting the data of matrix A and B
call InPut2D_F(flna//fs, nglobal, 1, mtrxA)
call Input1D_F(flnb//fs, nglobal, 1, mtrxB)
! make a copy for serial Gaussian elimination
mtrxA1 = mtrxA
mtrxB1 = mtrxB
! call the subroutine for serial Gaussian elimination
call serial_gaussian(mtrxA1, nglobal, mtrxB1, mtrxX1)
! initialize mpi
call MPI_INIT(ierr)
! get the total number of ranks and identify each rank
call MPI_COMM_RANK(MPI_COMM_WORLD, myRank, ierr)
call MPI_COMM_SIZE(MPI_COMM_WORLD, numRanks, ierr)

! determine the local size for each rank
if(mod(nglobal,numRanks) > myRank) then
  nlocal = nglobal/numRanks + 1
else
  nlocal = nglobal/numRanks
endif
! record the starting time
time1 = MPI_Wtime()
! allocate space for local matrix, the last element of mtrx_A is the row number in
! mtrxA, the original one before domain decomposition
ALLOCATE(mtrx_A(nlocal, nglobal+1), mtrx_B(nlocal), mtrx_X(nlocal))
! distribute the matrix by cyclical mapping
Do i = 1, nlocal
  mtrx_A(i, 1:nglobal) = mtrxA((i-1)*numRanks+myRank+1, 1:nglobal)
  mtrx_A(i,nglobal+1) = (i-1)*numRanks+myRank+1
  mtrx_B(i) = mtrxB((i-1)*numRanks+myRank+1)
ENDDO
! allocate space for pivot row
ALLOCATE(tempA(nglobal),tempB(1))
! do the forward elimination
DO i = 1, nglobal-1
  if(mtrx_A((i-1)/numRanks+1,nglobal+1) == i) then
    tempA = mtrx_A((i-1)/numRanks+1, 1:nglobal)
    tempB = mtrx_B((i-1)/numRanks+1)
  endif
! broadcast the pivot row to each rank
  CALL MPI_Bcast(tempA, nglobal, MPI_DOUBLE, MOD(i-1,numRanks), MPI_COMM_WORLD, ierr)
  CALL MPI_Bcast(tempB, 1, MPI_DOUBLE, MOD(i-1,numRanks), MPI_COMM_WORLD, ierr)
! perform the elimination
  DO j = 1, nlocal
    if(mtrx_A(j,nglobal+1) > i)then
      mtrx_B(j) = mtrx_B(j) - mtrx_A(j,i)/tempA(i)*tempB(1)
      mtrx_A(j, i:nglobal) = mtrx_A(j, i:nglobal) - mtrx_A(j,i)/tempA(i)*tempA(i:nglobal)
      mtrx_A(j,i) = 0.0
    endif
  ENDDO
ENDDO
! do the backward elimination
DO i = nglobal, 2, -1
  if(mtrx_A((i-1)/numRanks+1,nglobal+1) == i) then
    tempA = mtrx_A((i-1)/numRanks+1, 1:nglobal)
    tempB = mtrx_B((i-1)/numRanks+1)
  endif
  CALL MPI_Bcast(tempA, nglobal, MPI_DOUBLE, MOD(i-1,numRanks), MPI_COMM_WORLD, ierr)
  CALL MPI_Bcast(tempB, 1, MPI_DOUBLE, MOD(i-1,numRanks), MPI_COMM_WORLD, ierr)
!perform the elimination
  DO j = 1, nlocal
    if(mtrx_A(j,nglobal+1) < i)then
      mtrx_B(j) = mtrx_B(j) - mtrx_A(j,i)/tempA(i)*tempB(1)
      mtrx_A(j, i:nglobal) = mtrx_A(j, i:nglobal) - mtrx_A(j,i)/tempA(i)*tempA(i:nglobal)
      mtrx_A(j,i) = 0.0
    endif
  ENDDO
ENDDO
! calculate the results
 do i = 1, nlocal
   mtrx_X(i) = mtrx_B(i)/mtrx_A(i,int(mtrx_A(i,nglobal+1)))
 enddo
! record the finishing time
time2 = MPI_Wtime()
timef = time2 - time1

!check the results with serial version
DO i = 1, nlocal
  diff = mtrx_X(i) - mtrxX1((i-1)*numRanks+myRank+1)
  if (diff > 1e-6 .or. diff < -1e-6) then
  print*, "the parallel answer is wrong!"
  STOP 1
  endif
ENDDO
! print*, "the parallel answer is correct!"
print*, "I am rank ", myRank, " of ", numRanks, "time=", timef
! output the results
if(myRank == 0) then
  call OutPut1D_F(flnx//fs, nglobal, 1, mtrxX1)
endif
call MPI_FINALIZE(ierr)

End Program Gauss_Elimination_v02

! subroutine for serial version
subroutine serial_gaussian(mtrxa, nsize, mtrxb, mtrxx)
implicit none
integer, intent(in) :: nsize
integer :: i, j, k
real(kind=8) :: sumV
real(kind=8), dimension(nsize) :: sfctor
real(kind=8), intent(inout), dimension(nsize,nsize) :: mtrxA
real(kind=8), intent(inout), dimension(nsize) :: mtrxB
real(kind=8), intent(out), dimension(nsize) :: mtrxX
sumV = 0.0
! forward elimination
Do k = 1, nsize-1
!eliminate the coloum
Do i = k+1, nsize
  sfctor(i) = mtrxA(i,k)/mtrxA(k,k)
    Do j = k+1, nsize
      mtrxA(i,j) = mtrxA(i,j) - sfctor(i)*mtrxA(k,j)
    ENDDO
     mtrxA(i,k) = 0
    mtrxB(i) = mtrxB(i) - sfctor(i)*mtrxB(k)
  ENDDO
ENDDO
! backward substitution
mtrxX(nsize) = 1/mtrxA(nsize,nsize)*mtrxB(nsize)
Do k = nsize-1, 1, -1
  sumV = 0.0
  Do j = k+1, nsize
    sumV = sumV + mtrxA(k,j)*mtrxX(j)
  ENDDO
 mtrxX(k) = 1/mtrxA(k,k)*(mtrxB(k) - sumV)
ENDDO
end subroutine serial_gaussian

! subroutine for input n-by-n matrix
SUBROUTINE InPut2D_F(prfn, n, AB, mtrxin)
IMPLICIT NONE
Integer, INTENT(in) :: n, AB
REAL(KIND=8),INTENT(OUT),DIMENSION(n,n) :: mtrxin
CHARACTER(LEN=10),INTENT(IN) :: prfn
CHARACTER(LEN=14) :: flnm
!! case 1 ==> ascii; case 2 ==> binary
SELECT CASE (AB)
	CASE (1)
		flnm = prfn//'.txt'
		OPEN(UNIT=20,FILE=flnm,FORM='FORMATTED',STATUS='OLD',ACTION='READ')
			READ(20,*) mtrxin(1:n,1:n)
		CLOSE(20)
	CASE (2)
		flnm = prfn//'.dat'
		OPEN(UNIT=20,FILE=flnm,FORM='UNFORMATTED',STATUS='OLD',ACTION='READ')
			READ(20) mtrxin(1:n,1:n)
		CLOSE(20)
END SELECT

END SUBROUTINE InPut2D_F
! subroutine for input n-by-one matrix
SUBROUTINE InPut1D_F(prfn, n, AB, mtrxin)
IMPLICIT NONE
Integer, INTENT(in) :: n, AB
REAL(KIND=8),INTENT(OUT),DIMENSION(n) :: mtrxin
CHARACTER(LEN=10),INTENT(IN) :: prfn
CHARACTER(LEN=14) :: flnm
!! case 1 ==> ascii; case 2 ==> binary
SELECT CASE (AB)
	CASE (1)
		flnm = prfn//'.txt'
		OPEN(UNIT=20,FILE=flnm,FORM='FORMATTED',STATUS='OLD',ACTION='READ')
			READ(20,*) mtrxin(1:n)
		CLOSE(20)
	CASE (2)
		flnm = prfn//'.dat'
		OPEN(UNIT=20,FILE=flnm,FORM='UNFORMATTED',STATUS='OLD',ACTION='READ')
			READ(20) mtrxin(1:n)
		CLOSE(20)
END SELECT
! subroutine for outputting data
END SUBROUTINE InPut1D_F

SUBROUTINE OutPut1D_F(prfn, n, AB, mtrxout)
IMPLICIT NONE

INTEGER,INTENT(IN) :: n, AB
REAL(KIND=8),INTENT(IN),DIMENSION(n) :: mtrxout
CHARACTER(LEN=10),INTENT(IN) :: prfn
CHARACTER(LEN=14) :: flnm

!! case 1 ==> ascii; case 2 ==> binary
SELECT CASE (AB)
	CASE (1)
		flnm = prfn//'.txt'
		OPEN(UNIT=20,FILE=flnm,FORM='FORMATTED',STATUS='REPLACE',ACTION='WRITE')
			WRITE(20,*) mtrxout(1:n)
		CLOSE(20)
	CASE (2)
		flnm = prfn//'.dat'
		OPEN(UNIT=20,FILE=flnm,FORM='UNFORMATTED',STATUS='REPLACE',ACTION='WRITE')
			WRITE(20) mtrxout(1:n)
		CLOSE(20)
END SELECT

END SUBROUTINE OutPut1D_F
