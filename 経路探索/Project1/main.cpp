#include "DxLib.h"
#include <malloc.h>

#define MOVESPEED			(20.0)			// �ړ����x
#define SPHERESIZE			(150.0f)		// ���̂̃T�C�Y
#define COLLWIDTH			(400.0f)		// �����蔻��̃T�C�Y

// �|���S�����m�̘A������ۑ�����ׂ̍\����
struct POLYLINKINFO
{
	int LinkPolyIndex[3];			// �|���S���̎O�̕ӂƂ��ꂼ��אڂ��Ă���|���S���̃|���S���ԍ�( -1�F�אڃ|���S������  -1�ȊO�F�|���S���ԍ� )
	float LinkPolyDistance[3];			// �אڂ��Ă���|���S���Ƃ̋���
	VECTOR CenterPosition;				// �|���S���̒��S���W
};

// �o�H�T�������p�̂P�|���S���̏��
struct PATHPLANNING_UNIT
{
	int PolyIndex;					// �|���S���ԍ�
	float TotalDistance;				// �o�H�T���ł��̃|���S���ɓ��B����܂łɒʉ߂����|���S���Ԃ̋����̍��v
	int PrevPolyIndex;				// �o�H�T���Ŋm�肵���o�H��̈�O�̃|���S��( ���|���S�����o�H��ɖ����ꍇ�� -1 )
	int NextPolyIndex;				// �o�H�T���Ŋm�肵���o�H��̈��̃|���S��( ���|���S�����o�H��ɖ����ꍇ�� -1 )
	PATHPLANNING_UNIT* ActiveNextUnit;		// �o�H�T�������ΏۂɂȂ��Ă��鎟�̃|���S���̃������A�h���X���i�[����ϐ�
};

// �o�H�T�������Ŏg�p�������ۑ�����ׂ̍\����
struct PATHPLANNING
{
	VECTOR StartPosition;				// �J�n�ʒu
	VECTOR GoalPosition;				// �ڕW�ʒu
	PATHPLANNING_UNIT* UnitArray;			// �o�H�T�������Ŏg�p����S�|���S���̏��z�񂪊i�[���ꂽ�������̈�̐擪�������A�h���X���i�[����ϐ�
	PATHPLANNING_UNIT* ActiveFirstUnit;		// �o�H�T�������ΏۂɂȂ��Ă���|���S���Q�̍ŏ��̃|���S�����ւ̃������A�h���X���i�[����ϐ�
	PATHPLANNING_UNIT* StartUnit;			// �o�H�̃X�^�[�g�n�_�ɂ���|���S�����ւ̃������A�h���X���i�[����ϐ�
	PATHPLANNING_UNIT* GoalUnit;			// �o�H�̃S�[���n�_�ɂ���|���S�����ւ̃������A�h���X���i�[����ϐ�
};

// �T�������o�H���ړ����鏈���Ɏg�p�������Z�߂��\����
struct PATHMOVEINFO
{
	int NowPolyIndex;				// ���ݏ���Ă���|���S���̔ԍ�
	VECTOR NowPosition;				// ���݈ʒu
	VECTOR MoveDirection;				// �ړ�����
	PATHPLANNING_UNIT* NowPathPlanningUnit;	// ���ݏ���Ă���|���S���̌o�H�T����񂪊i�[����Ă��郁�����A�h���X���i�[����ϐ�
	PATHPLANNING_UNIT* TargetPathPlanningUnit;	// ���̒��Ԓn�_�ƂȂ�o�H��̃|���S���̌o�H�T����񂪊i�[����Ă��郁�����A�h���X���i�[����ϐ�
};


int StageModelHandle;			// �X�e�[�W���f���n���h��
MV1_REF_POLYGONLIST PolyList;		// �X�e�[�W���f���̃|���S�����

POLYLINKINFO* PolyLinkInfo;		// �X�e�[�W���f���̑S�|���S�����́u�|���S�����m�̘A�����v�̔z�񂪊i�[���i�[���ꂽ�������G���A�̐擪�A�h���X���i�[����ϐ�
PATHPLANNING PathPlanning;		// �o�H�T�������p�̍\����
PATHMOVEINFO PathMove;			// �T�������o�H���ړ����鏈���Ɏg�p�������Z�߂��\����


int CheckOnPolyIndex(VECTOR Pos);						// �w��̍��W�̒����A�Ⴕ���͒���ɂ���|���S���̔ԍ����擾����( �|���S�������������ꍇ�� -1 ��Ԃ� )

void SetupPolyLinkInfo(void);						// �|���S�����m�̘A�������\�z����
void TerminatePolyLinkInfo(void);						// �|���S�����m�̘A�����̌�n�����s��
bool CheckPolyMove(VECTOR StartPos, VECTOR TargetPos);			// �|���S�����m�̘A�������g�p���Ďw��̓�̍��W�Ԃ𒼐��I�Ɉړ��ł��邩�ǂ������`�F�b�N����( �߂�l  true:�����I�Ɉړ��ł���  false:�����I�Ɉړ��ł��Ȃ� )
bool CheckPolyMoveWidth(VECTOR StartPos, VECTOR TargetPos, float Width);	// �|���S�����m�̘A�������g�p���Ďw��̓�̍��W�Ԃ𒼐��I�Ɉړ��ł��邩�ǂ������`�F�b�N����( �߂�l  true:�����I�Ɉړ��ł���  false:�����I�Ɉړ��ł��Ȃ� )( ���w��� )

bool SetupPathPlanning(VECTOR StartPos, VECTOR GoalPos);			// �w��̂Q�_�̌o�H��T������( �߂�l  true:�o�H�\�z����  false:�o�H�\�z���s( �X�^�[�g�n�_�ƃS�[���n�_���q���o�H������������ ) )
void TerminatePathPlanning(void);						// �o�H�T�����̌�n��

void MoveInitialize(void);							// �T�������o�H���ړ����鏈���̏��������s���֐�
void MoveProcess(void);							// �T�������o�H���ړ����鏈���̂P�t���[�����̏������s���֐�
bool RefreshMoveDirection(void);						// �T�������o�H���ړ����鏈���ňړ��������X�V���鏈�����s���֐�( �߂�l  true:�S�[���ɒH�蒅���Ă���  false:�S�[���ɒH�蒅���Ă��Ȃ� )



// �w��̍��W�̒����A�Ⴕ���͒���ɂ���|���S���̔ԍ����擾����( �|���S�������������ꍇ�� -1 ��Ԃ� )
int CheckOnPolyIndex(VECTOR Pos)
{
	int i;
	VECTOR LinePos1;
	VECTOR LinePos2;
	HITRESULT_LINE HitRes;
	MV1_REF_POLYGON* RefPoly;

	// �w��̍��W��Y�������ɑ傫���L�т�����̂Q���W���Z�b�g
	LinePos1 = VGet(Pos.x, 1000000.0f, Pos.z);
	LinePos2 = VGet(Pos.x, -1000000.0f, Pos.z);

	// �X�e�[�W���f���̃|���S���̐������J��Ԃ�
	RefPoly = PolyList.Polygons;
	for (i = 0; i < PolyList.PolygonNum; i++, RefPoly++)
	{
		// �����Ɛڂ���|���S�����������炻�̃|���S���̔ԍ���Ԃ�
		HitRes = HitCheck_Line_Triangle(
			LinePos1,
			LinePos2,
			PolyList.Vertexs[RefPoly->VIndex[0]].Position,
			PolyList.Vertexs[RefPoly->VIndex[1]].Position,
			PolyList.Vertexs[RefPoly->VIndex[2]].Position
		);
		if (HitRes.HitFlag)
		{
			return i;
		}
	}

	// �����ɗ���������Ɛڂ���|���S�������������Ƃ������ƂȂ̂� -1 ��Ԃ�
	return -1;
}


// �|���S�����m�̘A�������\�z����
void SetupPolyLinkInfo(void)
{
	int i;
	int j;
	POLYLINKINFO* PLInfo;			//�|���S���A�����L��
	POLYLINKINFO* PLInfoSub;		//�|���S���A�����L��(�T�u)
	MV1_REF_POLYGON* RefPoly;		
	MV1_REF_POLYGON* RefPolySub;

	// �X�e�[�W���f���S�̂̎Q�Ɨp���b�V�����\�z����
	MV1SetupReferenceMesh(StageModelHandle, 0, TRUE);

	// �X�e�[�W���f���S�̂̎Q�Ɨp���b�V���̏����擾����
	PolyList = MV1GetReferenceMesh(StageModelHandle, 0, TRUE);

	// �X�e�[�W���f���̑S�|���S���̘A�������i�[����ׂ̃������̈���m�ۂ���
	// malloc ���I���������m�ۂ���
	//POLYLINKINDO*�^��POLYLINKINFO * PolyList.PolygonNum���̃������̈���m�ۂ���
	PolyLinkInfo = (POLYLINKINFO*)malloc(sizeof(POLYLINKINFO) * PolyList.PolygonNum);

	// �S�|���S���̒��S���W���Z�o
	PLInfo = PolyLinkInfo;
	RefPoly = PolyList.Polygons;
	for (i = 0; i < PolyList.PolygonNum; i++, PLInfo++, RefPoly++)
	{
		//�|���S���̒��S���W���Z�o���邽�߂�
		//�e3�̒��_�̍��W�𑫂���1/3����
		PLInfo->CenterPosition =
			VScale(VAdd(PolyList.Vertexs[RefPoly->VIndex[0]].Position,
				VAdd(PolyList.Vertexs[RefPoly->VIndex[1]].Position,
					PolyList.Vertexs[RefPoly->VIndex[2]].Position)), 1.0f / 3.0f);
	}

	// �|���S�����m�̗אڏ��̍\�z
	PLInfo = PolyLinkInfo;
	RefPoly = PolyList.Polygons;
	for (i = 0; i < PolyList.PolygonNum; i++, PLInfo++, RefPoly++)
	{
		// �e�ӂɗאڃ|���S���͖����A�̏�Ԃɂ��Ă���
		PLInfo->LinkPolyIndex[0] = -1;
		PLInfo->LinkPolyIndex[1] = -1;
		PLInfo->LinkPolyIndex[2] = -1;

		// �אڂ���|���S����T�����߂Ƀ|���S���̐������J��Ԃ�
		RefPolySub = PolyList.Polygons;
		PLInfoSub = PolyLinkInfo;
		for (j = 0; j < PolyList.PolygonNum; j++, RefPolySub++, PLInfoSub++)
		{
			// �������g�̃|���S���������牽���������̃|���S����
			if (i == j)
				continue;

			// �|���S���̒��_�ԍ�0��1�Ō`������ӂƗאڂ��Ă�����אڏ��ɒǉ�����
			// RefPoly->VIndex�Ƃ̓|���S�����`�����邽�߂̎O�̒��_�̂����̈�ł���
			if (PLInfo->LinkPolyIndex[0] == -1 &&
				((RefPoly->VIndex[0] == RefPolySub->VIndex[0] && RefPoly->VIndex[1] == RefPolySub->VIndex[2]) ||
					(RefPoly->VIndex[0] == RefPolySub->VIndex[1] && RefPoly->VIndex[1] == RefPolySub->VIndex[0]) ||
					(RefPoly->VIndex[0] == RefPolySub->VIndex[2] && RefPoly->VIndex[1] == RefPolySub->VIndex[1])))
			{
				PLInfo->LinkPolyIndex[0] = j;
				PLInfo->LinkPolyDistance[0] = VSize(VSub(PLInfoSub->CenterPosition, PLInfo->CenterPosition));
			}
			else
				// �|���S���̒��_�ԍ�1��2�Ō`������ӂƗאڂ��Ă�����אڏ��ɒǉ�����
				if (PLInfo->LinkPolyIndex[1] == -1 &&
					((RefPoly->VIndex[1] == RefPolySub->VIndex[0] && RefPoly->VIndex[2] == RefPolySub->VIndex[2]) ||
						(RefPoly->VIndex[1] == RefPolySub->VIndex[1] && RefPoly->VIndex[2] == RefPolySub->VIndex[0]) ||
						(RefPoly->VIndex[1] == RefPolySub->VIndex[2] && RefPoly->VIndex[2] == RefPolySub->VIndex[1])))
				{
					PLInfo->LinkPolyIndex[1] = j;
					PLInfo->LinkPolyDistance[1] = VSize(VSub(PLInfoSub->CenterPosition, PLInfo->CenterPosition));
				}
				else
					// �|���S���̒��_�ԍ�2��0�Ō`������ӂƗאڂ��Ă�����אڏ��ɒǉ�����
					if (PLInfo->LinkPolyIndex[2] == -1 &&
						((RefPoly->VIndex[2] == RefPolySub->VIndex[0] && RefPoly->VIndex[0] == RefPolySub->VIndex[2]) ||
							(RefPoly->VIndex[2] == RefPolySub->VIndex[1] && RefPoly->VIndex[0] == RefPolySub->VIndex[0]) ||
							(RefPoly->VIndex[2] == RefPolySub->VIndex[2] && RefPoly->VIndex[0] == RefPolySub->VIndex[1])))
					{
						PLInfo->LinkPolyIndex[2] = j;
						PLInfo->LinkPolyDistance[2] = VSize(VSub(PLInfoSub->CenterPosition, PLInfo->CenterPosition));
					}
		}
	}
}

// �|���S�����m�̘A�����̌�n�����s��
void TerminatePolyLinkInfo(void)
{
	// �|���S�����m�̘A�������i�[���Ă����������̈�����
	free(PolyLinkInfo);
	PolyLinkInfo = NULL;
}

// �|���S�����m�̘A�������g�p���Ďw��̓�̍��W�Ԃ𒼐��I�Ɉړ��ł��邩�ǂ������`�F�b�N����( �߂�l  true:�����I�Ɉړ��ł���  false:�����I�Ɉړ��ł��Ȃ� )
bool CheckPolyMove(VECTOR StartPos, VECTOR TargetPos)
{
	int StartPoly;		//�ړ��J�n���ɐG��Ă���|���S���ԍ�
	int TargetPoly;		//�S�[���̏ꏊ�ɐG��Ă���|���S���ԍ�
	POLYLINKINFO* PInfoStart;
	POLYLINKINFO* PInfoTarget;
	int i, j;
	VECTOR_D StartPosD = VGetD(0.0f,0.0f,0.0f);
	VECTOR_D TargetPosD = VGetD(0.0f, 0.0f, 0.0f);
	VECTOR_D PolyPos[3];
	int CheckPoly[3];
	int CheckPolyPrev[3];
	int CheckPolyNum;
	int CheckPolyPrevNum;
	int NextCheckPoly[3];
	int NextCheckPolyPrev[3];
	int NextCheckPolyNum;
	int NextCheckPolyPrevNum;

	// �J�n���W�ƖڕW���W�� y���W�l�� 0.0f �ɂ��āA���ʏ�̔���ɂ���
	StartPos.y = 0.0f;
	TargetPos.y = 0.0f;

	// ���x���グ�邽�߂� double�^�ɂ���
	StartPosD = VConvFtoD(StartPos);
	TargetPosD = VConvFtoD(TargetPos);

	// �J�n���W�ƖڕW���W�̒���A�Ⴕ���͒����ɑ��݂���|���S������������
	StartPoly = CheckOnPolyIndex(StartPos);
	TargetPoly = CheckOnPolyIndex(TargetPos);

	// �|���S�������݂��Ȃ�������ړ��ł��Ȃ��̂� false ��Ԃ�
	if (StartPoly == -1 || TargetPoly == -1)
		return false;

	// �J�n���W�ƖڕW���W�̒���A�Ⴕ���͒����ɑ��݂���|���S���̘A�����̃A�h���X���擾���Ă���
	PInfoStart = &PolyLinkInfo[StartPoly];
	PInfoTarget = &PolyLinkInfo[TargetPoly];

	// �w�������ɂ��邩�ǂ������`�F�b�N����|���S���Ƃ��ĊJ�n���W�̒���A�Ⴕ���͒����ɑ��݂���|���S����o�^
	CheckPolyNum = 1;
	CheckPoly[0] = StartPoly;
	CheckPolyPrevNum = 0;
	CheckPolyPrev[0] = -1;

	// ���ʂ��o��܂Ŗ������ŌJ��Ԃ�
	for (;;)
	{
		// ���̃��[�v�Ń`�F�b�N�ΏۂɂȂ�|���S���̐������Z�b�g���Ă���
		NextCheckPolyNum = 0;

		// ���̃��[�v�Ń`�F�b�N�Ώۂ���O���|���S���̐������Z�b�g���Ă���
		NextCheckPolyPrevNum = 0;

		// �`�F�b�N�Ώۂ̃|���S���̐������J��Ԃ�
		for (i = 0; i < CheckPolyNum; i++)
		{
			// �`�F�b�N�Ώۂ̃|���S���̂R���W���擾
			PolyPos[0] = VConvFtoD(PolyList.Vertexs[PolyList.Polygons[CheckPoly[i]].VIndex[0]].Position);
			PolyPos[1] = VConvFtoD(PolyList.Vertexs[PolyList.Polygons[CheckPoly[i]].VIndex[1]].Position);
			PolyPos[2] = VConvFtoD(PolyList.Vertexs[PolyList.Polygons[CheckPoly[i]].VIndex[2]].Position);

			// y���W��0.0�ɂ��āA���ʓI�Ȕ�����s���悤�ɂ���
			PolyPos[0].y = 0.0;
			PolyPos[1].y = 0.0;
			PolyPos[2].y = 0.0;

			// �|���S���̒��_�ԍ�0��1�̕ӂɗאڂ���|���S�������݂���ꍇ�ŁA
			// ���ӂ̐����ƈړ��J�n�_�A�I���_�Ō`������������ڂ��Ă����� if �����^�ɂȂ�
			if (PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[0] != -1 &&
				Segment_Segment_MinLength_SquareD(StartPosD, TargetPosD, PolyPos[0], PolyPos[1]) < 0.001)
			{
				// �����ӂƐڂ��Ă���|���S�����ڕW���W��ɑ��݂���|���S����������
				// �J�n���W����ڕW���W��܂œr�؂�Ȃ��|���S�������݂���Ƃ������ƂȂ̂� true ��Ԃ�
				if (PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[0] == TargetPoly)
					return true;

				// �ӂƐڂ��Ă���|���S�������̃`�F�b�N�Ώۂ̃|���S���ɉ�����

				// ���ɓo�^����Ă���|���S���̏ꍇ�͉����Ȃ�
				for (j = 0; j < NextCheckPolyNum; j++)
				{
					if (NextCheckPoly[j] == PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[0])
						break;
				}
				if (j == NextCheckPolyNum)
				{
					// ���̃��[�v�ŏ��O����|���S���̑Ώۂɉ�����

					// ���ɓo�^����Ă��鏜�O�|���S���̏ꍇ�͉����Ȃ�
					for (j = 0; j < NextCheckPolyPrevNum; j++)
					{
						if (NextCheckPolyPrev[j] == CheckPoly[i])
							break;
					}
					if (j == NextCheckPolyPrevNum)
					{
						NextCheckPolyPrev[NextCheckPolyPrevNum] = CheckPoly[i];
						NextCheckPolyPrevNum++;
					}

					// ��O�̃��[�v�Ń`�F�b�N�ΏۂɂȂ����|���S���̏ꍇ�������Ȃ�
					for (j = 0; j < CheckPolyPrevNum; j++)
					{
						if (CheckPolyPrev[j] == PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[0])
							break;
					}
					if (j == CheckPolyPrevNum)
					{
						// �����܂ŗ�����Q�����̃`�F�b�N�Ώۂ̃|���S���ɉ�����
						NextCheckPoly[NextCheckPolyNum] = PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[0];
						NextCheckPolyNum++;
					}
				}
			}

			// �|���S���̒��_�ԍ�1��2�̕ӂɗאڂ���|���S�������݂���ꍇ�ŁA
			// ���ӂ̐����ƈړ��J�n�_�A�I���_�Ō`������������ڂ��Ă����� if �����^�ɂȂ�
			if (PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[1] != -1 &&
				Segment_Segment_MinLength_SquareD(StartPosD, TargetPosD, PolyPos[1], PolyPos[2]) < 0.001)
			{
				// �����ӂƐڂ��Ă���|���S�����ڕW���W��ɑ��݂���|���S����������
				// �J�n���W����ڕW���W��܂œr�؂�Ȃ��|���S�������݂���Ƃ������ƂȂ̂� true ��Ԃ�
				if (PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[1] == TargetPoly)
					return true;

				// �ӂƐڂ��Ă���|���S�������̃`�F�b�N�Ώۂ̃|���S���ɉ�����

				// ���ɓo�^����Ă���|���S���̏ꍇ�͉����Ȃ�
				for (j = 0; j < NextCheckPolyNum; j++)
				{
					if (NextCheckPoly[j] == PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[1])
						break;
				}
				if (j == NextCheckPolyNum)
				{
					// ���ɓo�^����Ă��鏜�O�|���S���̏ꍇ�͉����Ȃ�
					for (j = 0; j < NextCheckPolyPrevNum; j++)
					{
						if (NextCheckPolyPrev[j] == CheckPoly[i])
							break;
					}
					if (j == NextCheckPolyPrevNum)
					{
						NextCheckPolyPrev[NextCheckPolyPrevNum] = CheckPoly[i];
						NextCheckPolyPrevNum++;
					}

					// ��O�̃��[�v�Ń`�F�b�N�ΏۂɂȂ����|���S���̏ꍇ�������Ȃ�
					for (j = 0; j < CheckPolyPrevNum; j++)
					{
						if (CheckPolyPrev[j] == PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[1])
							break;
					}
					if (j == CheckPolyPrevNum)
					{
						// �����܂ŗ�����Q�����̃`�F�b�N�Ώۂ̃|���S���ɉ�����
						NextCheckPoly[NextCheckPolyNum] = PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[1];
						NextCheckPolyNum++;
					}
				}
			}

			// �|���S���̒��_�ԍ�2��0�̕ӂɗאڂ���|���S�������݂���ꍇ�ŁA
			// ���ӂ̐����ƈړ��J�n�_�A�I���_�Ō`������������ڂ��Ă����� if �����^�ɂȂ�
			if (PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[2] != -1 &&
				Segment_Segment_MinLength_SquareD(StartPosD, TargetPosD, PolyPos[2], PolyPos[0]) < 0.001)
			{
				// �����ӂƐڂ��Ă���|���S�����ڕW���W��ɑ��݂���|���S����������
				// �J�n���W����ڕW���W��܂œr�؂�Ȃ��|���S�������݂���Ƃ������ƂȂ̂� true ��Ԃ�
				if (PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[2] == TargetPoly)
					return true;

				// �ӂƐڂ��Ă���|���S�������̃`�F�b�N�Ώۂ̃|���S���ɉ�����

				// ���ɓo�^����Ă���|���S���̏ꍇ�͉����Ȃ�
				for (j = 0; j < NextCheckPolyNum; j++)
				{
					if (NextCheckPoly[j] == PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[2])
						break;
				}
				if (j == NextCheckPolyNum)
				{
					// ���ɓo�^����Ă��鏜�O�|���S���̏ꍇ�͉����Ȃ�
					for (j = 0; j < NextCheckPolyPrevNum; j++)
					{
						if (NextCheckPolyPrev[j] == CheckPoly[i])
							break;
					}
					if (j == NextCheckPolyPrevNum)
					{
						NextCheckPolyPrev[NextCheckPolyPrevNum] = CheckPoly[i];
						NextCheckPolyPrevNum++;
					}

					// ��O�̃��[�v�Ń`�F�b�N�ΏۂɂȂ����|���S���̏ꍇ�������Ȃ�
					for (j = 0; j < CheckPolyPrevNum; j++)
					{
						if (CheckPolyPrev[j] == PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[2])
							break;
					}
					if (j == CheckPolyPrevNum)
					{
						// �����܂ŗ�����Q�����̃`�F�b�N�Ώۂ̃|���S���ɉ�����
						NextCheckPoly[NextCheckPolyNum] = PolyLinkInfo[CheckPoly[i]].LinkPolyIndex[2];
						NextCheckPolyNum++;
					}
				}
			}
		}

		// ���̃��[�v�Ń`�F�b�N�ΏۂɂȂ�|���S��������Ȃ������Ƃ������Ƃ�
		// �ړ��J�n�_�A�I���_�Ō`����������Ɛڂ���`�F�b�N�Ώۂ̃|���S���ɗאڂ���
		// �|���S��������Ȃ������Ƃ������ƂȂ̂ŁA�����I�Ȉړ��͂ł��Ȃ��Ƃ������Ƃ� false ��Ԃ�
		if (NextCheckPolyNum == 0)
		{
			return false;
		}

		// ���Ƀ`�F�b�N�ΏۂƂȂ�|���S���̏����R�s�[����
		for (i = 0; i < NextCheckPolyNum; i++)
		{
			CheckPoly[i] = NextCheckPoly[i];
		}
		CheckPolyNum = NextCheckPolyNum;

		// ���Ƀ`�F�b�N�ΏۊO�ƂȂ�|���S���̏����R�s�[����
		for (i = 0; i < NextCheckPolyPrevNum; i++)
		{
			CheckPolyPrev[i] = NextCheckPolyPrev[i];
		}
		CheckPolyPrevNum = NextCheckPolyPrevNum;
	}
}

// �|���S�����m�̘A�������g�p���Ďw��̓�̍��W�Ԃ𒼐��I�Ɉړ��ł��邩�ǂ������`�F�b�N����( �߂�l  true:�����I�Ɉړ��ł���  false:�����I�Ɉړ��ł��Ȃ� )( ���w��� )
bool CheckPolyMoveWidth(VECTOR StartPos, VECTOR TargetPos, float Width)
{
	VECTOR Direction;
	VECTOR SideDirection;
	VECTOR TempVec;

	// �ŏ��ɊJ�n���W����ڕW���W�ɒ����I�Ɉړ��ł��邩�ǂ������`�F�b�N
	if (CheckPolyMove(StartPos, TargetPos) == false)
		return false;

	// �J�n���W����ڕW���W�Ɍ������x�N�g�����Z�o
	Direction = VSub(TargetPos, StartPos);

	// y���W�� 0.0f �ɂ��ĕ��ʓI�ȃx�N�g���ɂ���
	Direction.y = 0.0f;

	// �J�n���W����ڕW���W�Ɍ������x�N�g���ɒ��p�Ȑ��K���x�N�g�����Z�o
	SideDirection = VCross(Direction, VGet(0.0f, 1.0f, 0.0f));
	SideDirection = VNorm(SideDirection);

	// �J�n���W�ƖڕW���W�� Width / 2.0f ���������������ɂ��炵�āA�ēx�����I�Ɉړ��ł��邩�ǂ������`�F�b�N
	TempVec = VScale(SideDirection, Width / 2.0f);
	if (CheckPolyMove(VAdd(StartPos, TempVec), VAdd(TargetPos, TempVec)) == false)
		return false;

	// �J�n���W�ƖڕW���W�� Width / 2.0f ��������O�Ƃ͋t�����̐��������ɂ��炵�āA�ēx�����I�Ɉړ��ł��邩�ǂ������`�F�b�N
	TempVec = VScale(SideDirection, -Width / 2.0f);
	if (CheckPolyMove(VAdd(StartPos, TempVec), VAdd(TargetPos, TempVec)) == false)
		return false;

	// �����܂ł�����w��̕��������Ă������I�Ɉړ��ł���Ƃ������ƂȂ̂� true ��Ԃ�
	return true;
}


// �w��̂Q�_�̌o�H��T������( �߂�l  true:�o�H�\�z����  false:�o�H�\�z���s( �X�^�[�g�n�_�ƃS�[���n�_���q���o�H������������ ) )
bool SetupPathPlanning(VECTOR StartPos, VECTOR GoalPos)
{
	int i;
	int PolyIndex;
	PATHPLANNING_UNIT* PUnit;
	PATHPLANNING_UNIT* PUnitSub;
	PATHPLANNING_UNIT* PUnitSub2;
	bool Goal;

	// �X�^�[�g�ʒu�ƃS�[���ʒu��ۑ�
	PathPlanning.StartPosition = StartPos;
	PathPlanning.GoalPosition = GoalPos;

	// �o�H�T���p�̃|���S�������i�[���郁�����̈���m�ۂ���
	PathPlanning.UnitArray = (PATHPLANNING_UNIT*)malloc(sizeof(PATHPLANNING_UNIT) * PolyList.PolygonNum);

	// �o�H�T���p�̃|���S������������
	PUnit = PathPlanning.UnitArray;
	for (i = 0; i < PolyList.PolygonNum; i++, PUnit++)
	{
		PUnit->PolyIndex = i;
		PUnit->TotalDistance = 0.0f;
		PUnit->PrevPolyIndex = -1;
		PUnit->NextPolyIndex = -1;
		PUnit->ActiveNextUnit = NULL;
	}

	// �X�^�[�g�n�_�ɂ���|���S���̔ԍ����擾���A�|���S���̌o�H�T�������p�̍\���̂̃A�h���X��ۑ�
	PolyIndex = CheckOnPolyIndex(StartPos);
	if (PolyIndex == -1)
		return false;
	PathPlanning.StartUnit = &PathPlanning.UnitArray[PolyIndex];

	// �o�H�T�������Ώۂ̃|���S���Ƃ��ăX�^�[�g�n�_�ɂ���|���S����o�^����
	PathPlanning.ActiveFirstUnit = &PathPlanning.UnitArray[PolyIndex];

	// �S�[���n�_�ɂ���|���S���̔ԍ����擾���A�|���S���̌o�H�T�������p�̍\���̂̃A�h���X��ۑ�
	PolyIndex = CheckOnPolyIndex(GoalPos);
	if (PolyIndex == -1)
		return false;
	PathPlanning.GoalUnit = &PathPlanning.UnitArray[PolyIndex];

	// �S�[���n�_�ɂ���|���S���ƃX�^�[�g�n�_�ɂ���|���S���������������� false ��Ԃ�
	if (PathPlanning.GoalUnit == PathPlanning.StartUnit)
		return false;

	// �o�H��T�����ăS�[���n�_�̃|���S���ɂ��ǂ蒅���܂Ń��[�v���J��Ԃ�
	Goal = false;
	while (Goal == false)
	{
		// �o�H�T�������ΏۂɂȂ��Ă���|���S�������ׂď���
		PUnit = PathPlanning.ActiveFirstUnit;
		PathPlanning.ActiveFirstUnit = NULL;
		for (; PUnit != NULL; PUnit = PUnit->ActiveNextUnit)
		{
			// �|���S���̕ӂ̐������J��Ԃ�
			for (i = 0; i < 3; i++)
			{
				// �ӂɗאڂ���|���S���������ꍇ�͉������Ȃ�
				if (PolyLinkInfo[PUnit->PolyIndex].LinkPolyIndex[i] == -1)
					continue;

				// �אڂ���|���S�������Ɍo�H�T���������s���Ă��āA����苗���̒����o�H�ƂȂ��Ă��邩�A
				// �X�^�[�g�n�_�̃|���S���������ꍇ�͉������Ȃ�
				PUnitSub = &PathPlanning.UnitArray[PolyLinkInfo[PUnit->PolyIndex].LinkPolyIndex[i]];
				if ((PUnitSub->PrevPolyIndex != -1 && PUnitSub->TotalDistance <= PUnit->TotalDistance + PolyLinkInfo[PUnit->PolyIndex].LinkPolyDistance[i])
					|| PUnitSub->PolyIndex == PathPlanning.StartUnit->PolyIndex)
					continue;

				// �אڂ���|���S�����S�[���n�_�ɂ���|���S����������S�[���ɒH�蒅�����t���O�𗧂Ă�
				if (PUnitSub->PolyIndex == PathPlanning.GoalUnit->PolyIndex)
				{
					Goal = true;
				}

				// �אڂ���|���S���Ɍo�H���ƂȂ鎩���̃|���S���̔ԍ���������
				PUnitSub->PrevPolyIndex = PUnit->PolyIndex;

				// �אڂ���|���S���ɂ����ɓ��B����܂ł̋�����������
				PUnitSub->TotalDistance = PUnit->TotalDistance + PolyLinkInfo[PUnit->PolyIndex].LinkPolyDistance[i];

				// ���̃��[�v�ōs���o�H�T�������Ώۂɒǉ�����A���ɒǉ�����Ă�����ǉ����Ȃ�
				for (PUnitSub2 = PathPlanning.ActiveFirstUnit; PUnitSub2 != NULL; PUnitSub2 = PUnitSub2->ActiveNextUnit)
				{
					if (PUnitSub2 == PUnitSub)
						break;
				}
				if (PUnitSub2 == NULL)
				{
					PUnitSub->ActiveNextUnit = PathPlanning.ActiveFirstUnit;
					PathPlanning.ActiveFirstUnit = PUnitSub;
				}
			}
		}

		// �����ɂ������� PathPlanning.ActiveFirstUnit �� NULL �Ƃ������Ƃ�
		// �X�^�[�g�n�_�ɂ���|���S������S�[���n�_�ɂ���|���S���ɒH�蒅���Ȃ��Ƃ������ƂȂ̂� false ��Ԃ�
		if (PathPlanning.ActiveFirstUnit == NULL)
		{
			return false;
		}
	}

	// �S�[���n�_�̃|���S������X�^�[�g�n�_�̃|���S���ɒH����
	// �o�H��̃|���S���Ɏ��Ɉړ����ׂ��|���S���̔ԍ���������
	PUnit = PathPlanning.GoalUnit;
	do
	{
		PUnitSub = PUnit;
		PUnit = &PathPlanning.UnitArray[PUnitSub->PrevPolyIndex];

		PUnit->NextPolyIndex = PUnitSub->PolyIndex;

	} while (PUnit != PathPlanning.StartUnit);

	// �����ɂ�����X�^�[�g�n�_����S�[���n�_�܂ł̌o�H���T���ł����Ƃ������ƂȂ̂� true ��Ԃ�
	return true;
}

// �o�H�T�����̌�n��
void TerminatePathPlanning(void)
{
	// �o�H�T���ׂ̈Ɋm�ۂ����������̈�����
	free(PathPlanning.UnitArray);
	PathPlanning.UnitArray = NULL;
}

// �T�������o�H���ړ����鏈���̏��������s���֐�
void MoveInitialize(void)
{
	// �ړ��J�n���_�ŏ���Ă���|���S���̓X�^�[�g�n�_�ɂ���|���S��
	PathMove.NowPolyIndex = PathPlanning.StartUnit->PolyIndex;

	// �ړ��J�n���_�̍��W�̓X�^�[�g�n�_�ɂ���|���S���̒��S���W
	PathMove.NowPosition = PolyLinkInfo[PathMove.NowPolyIndex].CenterPosition;

	// �ړ��J�n���_�̌o�H�T�����̓X�^�[�g�n�_�ɂ���|���S���̏��
	PathMove.NowPathPlanningUnit = PathPlanning.StartUnit;

	// �ړ��J�n���_�̈ړ����Ԓn�_�̌o�H�T�������X�^�[�g�n�_�ɂ���|���S���̏��
	PathMove.TargetPathPlanningUnit = PathPlanning.StartUnit;
}

// �T�������o�H���ړ����鏈���̂P�t���[�����̏������s���֐�
void MoveProcess(void)
{
	// �ړ������̍X�V�A�S�[���ɒH�蒅���Ă�����ړ��͂����ɏI������
	if (RefreshMoveDirection())
		return;

	// �ړ������ɍ��W���ړ�����
	PathMove.NowPosition = VAdd(PathMove.NowPosition, VScale(PathMove.MoveDirection, MOVESPEED));

	// ���݂̍��W�ŏ���Ă���|���S������������
	PathMove.NowPolyIndex = CheckOnPolyIndex(PathMove.NowPosition);

	// ���݂̍��W�ŏ���Ă���|���S���̌o�H�T�����̃������A�h���X��������
	PathMove.NowPathPlanningUnit = &PathPlanning.UnitArray[PathMove.NowPolyIndex];
}

// �T�������o�H���ړ����鏈���ňړ��������X�V���鏈�����s���֐�( �߂�l  true:�S�[���ɒH�蒅���Ă���  false:�S�[���ɒH�蒅���Ă��Ȃ� )
bool RefreshMoveDirection(void)
{
	PATHPLANNING_UNIT* TempPUnit;

	// ���ݏ���Ă���|���S�����S�[���n�_�ɂ���|���S���̏ꍇ�͏����𕪊�
	if (PathMove.NowPathPlanningUnit == PathPlanning.GoalUnit)
	{
		// �����͖ڕW���W
		PathMove.MoveDirection = VSub(PathPlanning.GoalPosition, PathMove.NowPosition);
		PathMove.MoveDirection.y = 0.0f;

		// �ڕW���W�܂ł̋������ړ����x�ȉ���������S�[���ɒH��������Ƃɂ���
		if (VSize(PathMove.MoveDirection) <= MOVESPEED)
		{
			return true;
		}

		// ����ȊO�̏ꍇ�͂܂����ǂ蒅���Ă��Ȃ����̂Ƃ��Ĉړ�����
		PathMove.MoveDirection = VNorm(PathMove.MoveDirection);

		return false;
	}

	// ���ݏ���Ă���|���S�����ړ����Ԓn�_�̃|���S���̏ꍇ�͎��̒��Ԓn�_�����肷�鏈�����s��
	if (PathMove.NowPathPlanningUnit == PathMove.TargetPathPlanningUnit)
	{
		// ���̒��Ԓn�_�����肷��܂Ń��[�v��������
		for (;;)
		{
			TempPUnit = &PathPlanning.UnitArray[PathMove.TargetPathPlanningUnit->NextPolyIndex];

			// �o�H��̎��̃|���S���̒��S���W�ɒ����I�Ɉړ��ł��Ȃ��ꍇ�̓��[�v���甲����
			if (CheckPolyMoveWidth(PathMove.NowPosition, PolyLinkInfo[TempPUnit->PolyIndex].CenterPosition, COLLWIDTH) == false)
				break;

			// �`�F�b�N�Ώۂ��o�H��̍X�Ɉ��̃|���S���ɕύX����
			PathMove.TargetPathPlanningUnit = TempPUnit;

			// �����S�[���n�_�̃|���S���������烋�[�v�𔲂���
			if (PathMove.TargetPathPlanningUnit == PathPlanning.GoalUnit)
				break;
		}
	}

	// �ړ����������肷��A�ړ������͌��݂̍��W���璆�Ԓn�_�̃|���S���̒��S���W�Ɍ���������
	PathMove.MoveDirection = VSub(PolyLinkInfo[PathMove.TargetPathPlanningUnit->PolyIndex].CenterPosition, PathMove.NowPosition);
	PathMove.MoveDirection.y = 0.0f;
	PathMove.MoveDirection = VNorm(PathMove.MoveDirection);

	// �����ɗ����Ƃ������Ƃ̓S�[���ɒH�蒅���Ă��Ȃ��̂� false ��Ԃ�
	return false;
}


// WinMain �֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	PATHPLANNING_UNIT* PUnit;

	// �E�C���h�E���[�h�ŋN��
	ChangeWindowMode(TRUE);

	// �c�w���C�u�����̏�����
	if (DxLib_Init() < 0) return -1;

	// �X�e�[�W���f���̓ǂݍ���
	StageModelHandle = MV1LoadModel(L"PathPlanning.mqo");

	// �X�e�[�W���f���̃|���S�����m�̘A�������\�z����
	SetupPolyLinkInfo();

	// �w��̂Q�_�̌o�H����T������
	SetupPathPlanning(VGet(-7400.0f, 0.0f, 7400.0f), VGet(7400.0f, 0.0f, 7400.0f));

	// �T�������o�H����ړ����鏀�����s��
	MoveInitialize();

	// �`���𗠉�ʂɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	// ���C�����[�v(�����L�[�������ꂽ�烋�[�v�𔲂���)
	while (ProcessMessage() == 0)
	{
		// ��ʂ̏�����
		ClearDrawScreen();

		// �P�t���[�����o�H����ړ�
		MoveProcess();

		// �J�����̐ݒ�
		SetCameraPositionAndTarget_UpVecY(VGet(-0.000f, 15692.565f, -3121.444f), VGet(0.000f, 0.000f, 0.000f));
		SetCameraNearFar(320.000f, 80000.000f);

		// �X�e�[�W���f����`�悷��
		MV1DrawModel(StageModelHandle);

		// �T�������o�H�̃|���S���̗֊s��`�悷��( �f�o�b�O�\�� )
		PUnit = PathPlanning.GoalUnit;
		for (;;)
		{
			DrawTriangle3D(
				PolyList.Vertexs[PolyList.Polygons[PUnit->PolyIndex].VIndex[0]].Position,
				PolyList.Vertexs[PolyList.Polygons[PUnit->PolyIndex].VIndex[1]].Position,
				PolyList.Vertexs[PolyList.Polygons[PUnit->PolyIndex].VIndex[2]].Position,
				GetColor(255, 0, 0),
				FALSE
			);

			if (PUnit->PrevPolyIndex == -1)
				break;

			PUnit = &PathPlanning.UnitArray[PUnit->PrevPolyIndex];
		}

		DrawFormatString(0, 0, 0xffffff, L"x:%fy:%fz:%f", 
			PathMove.NowPosition.x, PathMove.NowPosition.y, PathMove.NowPosition.z);
		// �ړ����̌��ݍ��W�ɋ��̂�`�悷��
		DrawSphere3D(VAdd(PathMove.NowPosition, VGet(0.0f, 40.0f, 0.0f)), SPHERESIZE, 10, GetColor(255, 0, 0), GetColor(0, 0, 0), TRUE);

		// ����ʂ̓��e��\��ʂɔ��f
		ScreenFlip();
	}

	// �o�H���̌�n��
	TerminatePathPlanning();

	// �X�e�[�W���f���̃|���S�����m�̘A�����̌�n��
	TerminatePolyLinkInfo();

	// �c�w���C�u�����̌�n��
	DxLib_End();

	// �\�t�g�̏I��
	return 0;
}