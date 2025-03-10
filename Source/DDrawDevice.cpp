/*
DirectDraw Tutorial Sample �ڵ忡�� ���� �߰��� �ҽ��ڵ� �κи� �־�ξ����ϴ�.
�Ʒ� �ҽ��ڵ�� ���� Tutorial�� 0�� ������ �����Լ��� ������� �Ͽ� ���� �����Ͽ� ���� �Լ��Դϴ�.
��������Ʈ �̹����� �����ϱ����ؼ� ���� �����Լ��� �ƴ� Ư���� Render�Լ��� �ʿ��Ͽ�����,
���� �¿������ �̹����� �����ϱ� ���ؼ��� Buffer�� ���� ���� �����ϴ� DirectDraw Ư���� �̿� �´� Render�Լ������� �ʿ��Ͽ����ϴ�.


1) CalcSpriteClipArea �Լ�
�Ʒ� 3������ �������

 - SpriteSheet ���� ���� ��ǥ�� �󿡼��� ��ǥ
 - ���� ȭ��(BackBuffer, Screen ��ǥ��) �󿡼��� ��ǥ
 - Screen ��ǥ�� ������ ����� ���� Clipping�� �Ǵµ�, Clipping ���� �׷����� Rect�� Width�� Height 


 2) DrawSprite �Լ�
 srcRect���� ������� SpriteSheet ������ �׷����� Sprite�̹����� Render�ϴ� �Լ�
 ��ü���� ������ �ּ����� �����


 3) DrawSpriteFlip �Լ�
 DrawSprite �Լ����� �¿������ �̹����� �׷��ִ� �Լ�
 ��ü���� ������ �ּ����� �����

*/



BOOL CDDrawDevice::CalcSpriteClipArea(
	INT_VECTOR2* pivOutSrcStart,      // ���� �ҽ� ���� ��ǥ (��������Ʈ ��Ʈ ��)
	INT_VECTOR2* pivOutDestStart,     // ���� ȭ����� ���� ��ǥ
	INT_VECTOR2* pivOutDestSize,      // ���� �������� ������ ũ��
	const RECT& srcRect,              // ��������Ʈ ��Ʈ �������� ������ ���� (��: ������ ����)
	const INT_VECTOR2* pDestPos,      // ȭ����� ������ ���� ��ǥ ({screenX, screenY})
	const INT_VECTOR2* pBufferSize)   // ȭ��(����) ũ�� ({width, height})
{

	// 1. srcRect�� ������� ���ϴ� ��������Ʈ ���� ũ�� ���
	INT_VECTOR2 desiredSize;
	desiredSize.x = srcRect.right - srcRect.left;
	desiredSize.y = srcRect.bottom - srcRect.top;

	// 2. ȭ����� ������ ���� Ŭ����: destPos�� desiredSize�� �̿��Ͽ� ���� �׸� ���� ����
	int destStartX = std::max<int>(pDestPos->x, 0);
	int destStartY = std::max<int>(pDestPos->y, 0);
	int destEndX = std::min<int>(pDestPos->x + desiredSize.x, pBufferSize->x);
	int destEndY = std::min<int>(pDestPos->y + desiredSize.y, pBufferSize->y);

	int clippedWidth = destEndX - destStartX;
	int clippedHeight = destEndY - destStartY;

	if (clippedWidth <= 0 || clippedHeight <= 0)
		return FALSE;			// �������� ������ ����

	// 3. ��������Ʈ ��Ʈ �� �ҽ� ���� ��ǥ ���:
	// ȭ����� ���� ��ǥ�� ���� destPos�� ���̸� srcRect�� ����/��ܿ� ����
	int srcStartX = destStartX - pDestPos->x + srcRect.left;
	int srcStartY = destStartY - pDestPos->y + srcRect.top;

	pivOutSrcStart->x = srcStartX;
	pivOutSrcStart->y = srcStartY;
	pivOutDestStart->x = destStartX;
	pivOutDestStart->y = destStartY;
	pivOutDestSize->x = clippedWidth;
	pivOutDestSize->y = clippedHeight;


	return TRUE;
}



BOOL CDDrawDevice::DrawSprite(int screenX, int screenY, const CImageData* pImgData, const RECT& srcRect)
{
	BOOL bResult = FALSE;

#ifdef _DEBUG
	if (!m_pWriteBuffer)
		__debugbreak();
#endif

	// ȭ���� �ʺ� (��� ���)
	int screenWidth = static_cast<int>(m_dwWidth);

	// ��������Ʈ �������� ũ�� ��� (srcRect ����)
	int frameWidth = srcRect.right - srcRect.left;
	int frameHeight = srcRect.bottom - srcRect.top;

	// ��������Ʈ ��Ʈ ��ü �̹����� ũ��
	/*
	int imageWidth = static_cast<int>(pImgData->GetWidth());
	int imageHeight = static_cast<int>(pImgData->GetHeight());
	*/

	// �������� �ҽ� ���� ��ǥ (��������Ʈ ��Ʈ ���� ��ǥ��)
	INT_VECTOR2 srcStart = { srcRect.left, srcRect.top };

	// ȭ����� ���� ��ǥ (��ũ�� ��ǥ��)
	INT_VECTOR2 destStart = { screenX, screenY };

	// ������ ������ ũ�� �ʱⰪ�� ��������Ʈ �������� ũ��� ����
	INT_VECTOR2 spriteSize = { frameWidth, frameHeight };
	INT_VECTOR2 clippedSize = {};

	// ��� ��ġ�� ���� ũ��
	INT_VECTOR2 destPos = { screenX, screenY };
	INT_VECTOR2 bufferSize = { screenWidth, static_cast<int>(m_dwHeight) };

	// CalcClipArea() �Լ��� destStart�� spriteSize�� �������
	// ������ ȭ�鿡 �׷��� ����(Ŭ���� ����)�� ���
	if (!CalcSpriteClipArea(&srcStart, &destStart, &clippedSize, srcRect, &destPos, &bufferSize))
		goto lb_return;


	// pImgData���� srcStart.y ���� ����� �����͸� ��������
	const COMPRESSED_LINE* lineDesc = pImgData->GetCompressedImage(srcStart.y);

	// ��� ������ ù �� ������ (destStart.y ����� ����)
	char* destLinePtr = m_pWriteBuffer + (destStart.y) * m_dwWriteBufferPitch;

	// Ŭ���� ������ ���̸�ŭ �ݺ�
	for (int y = 0; y < clippedSize.y; y++)
	{
		// �� ���� ��Ʈ�� �����͸� ��ȸ
		for (DWORD i = 0; i < lineDesc->dwStreamNum; i++)
		{
			PIXEL_STREAM* pStream = lineDesc->pPixelStream + i;
			DWORD pixelColor = pStream->dwPixel;
			int pixelCount = static_cast<int>(pStream->wPixelNum);

		


			/*
			�̴� 0�����࿡�� ��ü Sprite�̹��� �� Rect�����ȿ� �ش��ϴ� �̹����� Render�ϱ� ���� Case����
			���� �Ʒ� Case���� ������� ������ ��������Ʈ ��Ʈ ��ü�� ��������
			
			pStream->wPosX �� ��ü �̹����� ������ǥ��󿡼��� ���ӵ� �ȼ��� ���۵Ǵ� X��ǥ�̶�� ���� ����ؾ���
			�� {wPosX, wPosX + pixelCount} ��ŭ�� ���� Line���� ���ӵ� �����̶�°�
			���� �¿� Screen������ ���� �׷����� ���� �����ϴ� ������ �����ϴ���, �� ���� �۾��� �߰��ؼ� ó���ؾ���

			�켱 ���� srcRect ���� �ȿ� ���� ���� X�� ���ؾ��� �� ����
			�� ������ǥ�� ������ �����ϱ�

			Case1)   wPosX, wPosX + pixelCount�� Rect���� ������ �Ѿ�� �� �翬�� Render�� �ʿ䰡 ����
			Case2)   wPosX�� �Ѿ�� wPosX + pixelCount�� Rect���� ���ʿ� ���� ��
					   �� ���, ���� ��ǥ�� Rect.left �� ���װ�, PixelCount���� (wPosX + pixelCount - Rect.left) �̰� �� ��

			Case3)	wPosX, wPosX + pixelCount�� Rect���� ������ �� �״�� ���
			Case4)	wPosX�� Rect����, wPosX + pixelCount�� Rect�ٱ���
					    �� ���, ������ǥ�� wPosX, pixelCount�� (Rect.Right - wPosX)

			Case5)	wPosX, wPosX + pixelCount �� �� Rect �ٱ����� �� Render�� �ʿ䰡 ����

			�̷��� Rect�ȿ��� �׷��� localRenderStartX, pixelCount�� ���س� ��
			�̸� ������� screenRenderX = screenX + localRenderStartX
			�̷������� �θ� �� �� ����

			���� ���� screenRenderX�� ȭ�� �������, ������踦 ����� �� ó���� �����ϸ� �� �� ����

			�߰��� Rect���� Local ��ǥ��� �������� �� (Rect.left�� �����)
			�� (srcStart.x - srcRect.left) ���� ����� ��� ȭ���� �������� clipping�� �Ͼ ���̹Ƿ� �̿� ���� ó���� �߰���
			
			*/

			
			// Rect ���� Local ��ǥ��� �����ؼ� �����ϱ� (srcRect.left�� ���ִ� ��)
			// �� �� (srcStart.x - srcRect.left) �� ����̸� clipping�� �Ͼ ���̹Ƿ�, �� �κ��� ����Ͽ� localX�� frameWidth�� ����
			// localX = (wPosX - srcRect.left) - (srcStart.x - srcRect.left) = wPosX - srcStart.x
			int localX = static_cast<int>(pStream->wPosX) - srcStart.x;			// ���� ���Ŀ� ���� srcRect.left�� �Ұŵ�
			frameWidth = srcRect.right - srcStart.x;
			

			// Case1, Case5: srcRect ������ ��� ���
			if (localX + pixelCount <= 0 || localX >= frameWidth)
				continue;

			
			// Case2: ������ ��� ���
			if (localX < 0) {
				pixelCount += localX; // localX�� ����
				localX = 0;
			}

			// Case4: �������� ��� ���
			if (localX + pixelCount > frameWidth) {
				pixelCount = frameWidth - localX;
			}


			// ���������� ȭ����� x ��ǥ = destStart.x + localX
			int drawX = destStart.x + localX;


			// ȭ�� ���� ��踦 ����� ����
			
			if (drawX < 0)
			{
				pixelCount += drawX;
				drawX = 0;
			}
			

			// ȭ�� ���� ��踦 ����� ����
			if (drawX + pixelCount > screenWidth)
			{
				pixelCount = screenWidth - drawX;
			}


			// ��� ���� �� �ش� �ȼ� ��ġ ��� (�ȼ��� 4Byte)
			char* destPixelPtr = destLinePtr + (drawX * 4);

			// �ش� ��Ʈ���� �ȼ����� ����
			for (int x = 0; x < pixelCount; x++)
			{
				*(DWORD*)destPixelPtr = pixelColor;
				destPixelPtr += 4;
			}
		}
		lineDesc++; // ���� ���� ���� ������
		destLinePtr += m_dwWriteBufferPitch; // ��� ������ ���� �� ������
	}
	bResult = TRUE;
lb_return:
	return bResult;
}


BOOL CDDrawDevice::DrawSpriteFlip(int screenX, int screenY, const CImageData* pImgData, const RECT& srcRect)
{
    BOOL bResult = FALSE;

#ifdef _DEBUG
    if (!m_pWriteBuffer)
        __debugbreak();
#endif

    // ȭ���� �ʺ� (��� ���)
    int screenWidth = static_cast<int>(m_dwWidth);

    // ��������Ʈ �������� ũ�� (srcRect ����)
    int frameWidth = srcRect.right - srcRect.left;
    int frameHeight = srcRect.bottom - srcRect.top;

    // �������� �ҽ� ���� ��ǥ (��������Ʈ ��Ʈ ���� ��ǥ��)
    INT_VECTOR2 srcStart = { srcRect.left, srcRect.top };

    // ȭ����� ���� ��ǥ (��ũ�� ��ǥ��)
    INT_VECTOR2 destStart = { screenX, screenY };

    // ������ ������ ũ��� ��������Ʈ �������� ũ��� �ʱ�ȭ
    INT_VECTOR2 spriteSize = { frameWidth, frameHeight };
    INT_VECTOR2 clippedSize = {};

    // ��� ��ġ�� ���� ũ��
    INT_VECTOR2 destPos = { screenX, screenY };
    INT_VECTOR2 bufferSize = { screenWidth, static_cast<int>(m_dwHeight) };

    // CalcClipArea() �Լ��� destStart�� spriteSize�� ������� ���� ȭ�鿡 �׷��� ����(Ŭ���� ����)�� ���
    if (!CalcSpriteClipArea(&srcStart, &destStart, &clippedSize, srcRect, &destPos, &bufferSize))
        goto lb_return;

    // pImgData���� srcStart.y ���� ���� �����͸� ������
    const COMPRESSED_LINE* lineDesc = pImgData->GetCompressedImage(srcStart.y);

    // ��� ������ ù �� ������ (destStart.y ����� ����)
    char* destLinePtr = m_pWriteBuffer + (destStart.y) * m_dwWriteBufferPitch;

    // Ŭ���� ������ ���̸�ŭ �ݺ�
    for (int y = 0; y < clippedSize.y; y++)
    {
        // �� ���� ����� ��Ʈ�� �����͸� ��ȸ
        for (DWORD i = 0; i < lineDesc->dwStreamNum; i++)
        {
            PIXEL_STREAM* pStream = lineDesc->pPixelStream + i;
            DWORD pixelColor = pStream->dwPixel;
            int pixelCount = static_cast<int>(pStream->wPixelNum);

			// Rect �»���� �������� �ϴ� ������ǥ�踣 �������� �����ؾ� ����¥�Ⱑ ����
			// ���� srcRect.left�� �������
			int localX = static_cast<int>(pStream->wPosX) - srcRect.left;
			// frameWidth�� srcRect�� �ʺ�
			frameWidth = srcRect.right - srcRect.left;

            // ���� ��Ʈ���� srcRect ������ ����� �ǳʶ�
            if (localX + pixelCount <= 0 || localX >= frameWidth)
                continue;

            // Case2: ������ ��� ���
            if (localX < 0) {
                pixelCount += localX; // localX�� ����
                localX = 0;
            }

            // Case4: �������� ��� ���
            if (localX + pixelCount > frameWidth) {
                pixelCount = frameWidth - localX;
            }

            // �¿� ���� ó��:
            // ���� �¿� ����� destStart.x + localX�� ���������,
            // ������ ��쿣 �ش� �������� ������ ���������� localX��ŭ ������ ��ġ�� �����ϰ�,
            // pixelCount��ŭ �ȼ��� ä��Ƿ�,
            // drawX = destStart.x + (frameWidth - localX - pixelCount) �� �Ǿ���Ѵ�.
            int drawX = destStart.x + (frameWidth - localX - pixelCount);
			
			// �̹����� ���⼭ ���� ��ǥ��ġ�� ���ʿ��� Clipping �� ��ŭ �Ű������
			// (srcStart.x - srcRect.left) �� ������ ���ʿ��� Clippiing�� �Ͼ ��Ȳ

			drawX -= (srcStart.x - srcRect.left);

            // ȭ�� ���� ��� Ŭ����
            if (drawX < 0)
            {
                pixelCount += drawX;
                drawX = 0;
            }

            // ȭ�� ���� ��� Ŭ����
            if (drawX + pixelCount > screenWidth)
            {
                pixelCount = screenWidth - drawX;
            }

            // ��� ���� �� �ش� �ȼ� ��ġ ��� (�ȼ��� 4Byte)
            // (������ �̹����� �����ʿ������� ä�����Ƿ�, ���⼭�� �Ϲ����� ������ �����ص� ������ ���� �ݺ� ���� �� ��������)
            char* destPixelPtr = destLinePtr + (drawX * 4);

            // ���� ������ �ȼ����� pixelCount��ŭ ����
            for (int x = 0; x < pixelCount; x++)
            {
                *(DWORD*)destPixelPtr = pixelColor;
                destPixelPtr += 4;
            }
        }
        lineDesc++; // ���� ���� ���� ������
        destLinePtr += m_dwWriteBufferPitch; // ��� ������ ���� �� ������
    }
    bResult = TRUE;
lb_return:
    return bResult;
}




