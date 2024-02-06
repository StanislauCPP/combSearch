#include "Subset.h"

struct DeckPack
{
	unsigned seqAmount;
	unsigned char* indSeq;							//indSeq - index sequence
	unsigned sequenceSize;
	unsigned char* deck;
	unsigned deckSize;
	int* inShape;
	int* baseInShape;
	int* outNodes;
	int* baseOutNodes;
	int* sumShapeArr;
};

void initDeckPack(int shape, struct DeckPack* deckPack)
{
	int i;

	deckPack->seqAmount = 0;

	if ((shape * shape) % 8)
		deckPack->sequenceSize = ((shape * shape) / 8) + 1;
	else
		deckPack->sequenceSize = (shape * shape) / 8;

	deckPack->deckSize = 1;
	for (i = 2; i <= shape; ++i)
		deckPack->deckSize = deckPack->deckSize * i;

	deckPack->deckSize = deckPack->deckSize * deckPack->sequenceSize;

	deckPack->indSeq = calloc(deckPack->sequenceSize, sizeof(unsigned char));
	deckPack->deck = calloc(deckPack->deckSize, sizeof(unsigned char));
	deckPack->baseInShape = calloc(shape, sizeof(int));
	deckPack->inShape = calloc(shape, sizeof(int));
	deckPack->baseOutNodes = calloc(shape, sizeof(int));
	deckPack->outNodes = calloc(shape, sizeof(int));
	deckPack->sumShapeArr = calloc(shape, sizeof(int));
}

int lastSubset(int shape)
{
	int lSubset = 0, i;

	for (i = shape; i < (shape * 2); ++i)
		lSubset = lSubset | (1 << i);

	return lSubset;
}

void fillDeck(unsigned settedBits, unsigned sBCount, unsigned range, struct DeckPack* deckPack)
{
	unsigned i = 0, j, byteNum;
	unsigned char* indSeq = deckPack->indSeq;
	unsigned char* deck = deckPack->deck;

	if (!settedBits)
	{
		for (i = deckPack->sequenceSize * deckPack->seqAmount, j = 0; j < deckPack->sequenceSize; ++j, ++i)
			deck[i] = indSeq[i % deckPack->sequenceSize];

		++(deckPack->seqAmount);
	}
	else
		for (i = 0; i < range; ++i)
		{
			if (settedBits & (1 << i))
			{
				byteNum = ((sBCount + i) / 8);

				indSeq[byteNum] = indSeq[byteNum] | (1 << ((sBCount + i) % 8));
				sBCount += range;
				settedBits = settedBits & (~(1 << i));

				fillDeck(settedBits, sBCount, range, deckPack);

				sBCount -= range;
				indSeq[byteNum] = indSeq[byteNum] & (~(1 << ((sBCount + i) % 8)));
				settedBits = settedBits | (1 << i);
			}
		}
}

void setBase(int subSet, int borderSet, int* baseInShape, int* baseOutNodes)
{
	int i = 0, iSI = 0, oNI = 0;			//iTI - internal Shape Index; oDI - out Node Index

	for (i; i < borderSet; ++i)
	{
		if (subSet & 1)
			baseInShape[iSI++] = i + 1;
		else
			baseOutNodes[oNI++] = i + 1;

		subSet = subSet >> 1;
	}
}

void setArrByIndexes(int shape, unsigned indSeqNum, struct DeckPack* deckPack, int* base)
{
	unsigned char* deck = deckPack->deck;
	unsigned ind = indSeqNum * deckPack->sequenceSize, i = 0, j = 0;
	int* workArr;

	if (base == deckPack->baseInShape)
		workArr = deckPack->inShape;
	else
		workArr = deckPack->outNodes;

	while (j < shape)
	{
		if ((deck[ind + (i / 8)] >> (i % 8)) & 1)
		{
			workArr[j] = base[i % shape];

			++j;
			i = (i - (i % shape) + shape - 1);
		}
		++i;
	}
}

unsigned char checkByShapeSum(int shape, struct DeckPack* deckPack)
{
	int i;
	int* inShape = deckPack->inShape;
	int* outNodes = deckPack->outNodes;
	int* sum = deckPack->sumShapeArr;

	for (i = 0; i < shape; ++i)
		sum[i] = inShape[i] + inShape[(i + 1) % shape];

	for (i = 0; i < (shape - 1); ++i)
		if ((sum[i] - sum[i + 1]) != (outNodes[i + 1] - outNodes[i]))
			return 0;

	printf("\n");
	for (i = 0; i < shape; ++i)
		printf("%d, %d, %d\t\t", outNodes[i], inShape[i], inShape[(i + 1) % shape]);
	printf("\n");

	return 1;
}

void checkTriplets(int shape, int subset, struct DeckPack* deckPack)
{
	unsigned i, j, shapeLimit, nodeLimit;
	setBase(subset, (shape * 2), deckPack->baseInShape, deckPack->baseOutNodes);

	nodeLimit = deckPack->deckSize / deckPack->sequenceSize;
	shapeLimit = nodeLimit / shape;
	for (i = 0; i < shapeLimit; ++i)
	{
		setArrByIndexes(shape, i, deckPack, deckPack->baseInShape);
		for (j = 0; j < nodeLimit; ++j)
		{
			setArrByIndexes(shape, j, deckPack, deckPack->baseOutNodes);

			if (checkByShapeSum(shape, deckPack))
				break;
		}
	}
}

void freeDeckPack(struct DeckPack* deckPack)
{
	free(deckPack->indSeq);
	deckPack->indSeq = NULL;
	free(deckPack->deck);
	deckPack->deck = NULL;
	free(deckPack->baseInShape);
	deckPack->baseInShape = NULL;
	free(deckPack->inShape);
	deckPack->inShape = NULL;
	free(deckPack->baseOutNodes);
	deckPack->baseOutNodes = NULL;
	free(deckPack->outNodes);
	deckPack->outNodes = NULL;
	free(deckPack->sumShapeArr);
	deckPack->sumShapeArr = NULL;

	deckPack->sequenceSize = 0;
	deckPack->deckSize = 0;
}

void getCombinationsByShape(int shape)
{
	struct DeckPack deckPack;
	int lSubset = 0, i = 0, j = 0, count = 0, tmpShape, settedBits;

	initDeckPack(shape, &deckPack);

	lSubset = lastSubset(shape);
	tmpShape = shape;

	while (tmpShape)
	{
		--tmpShape;
		i = i | (1 << tmpShape);
	}

	settedBits = i;
	fillDeck(settedBits, 0, shape, &deckPack);

	for (i; i <= lSubset; ++i)
	{
		for (j = i, count = 0; j > 0; j = j & (j - 1))
		{
			++count;
			if (count > shape)
				break;
		}

		if (count == shape)
			checkTriplets(shape, i, &deckPack);
	}

	freeDeckPack(&deckPack);
}

