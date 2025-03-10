/*
DirectDraw Tutorial Sample 코드에서 제가 추가한 소스코드 부분만 넣어두었습니다.
아래 소스코드는 기존 Tutorial의 0번 압축기반 렌더함수를 기반으로 하여 조금 응용하여 만든 함수입니다.
스프라이트 이미지를 렌더하기위해선 기존 렌더함수가 아닌 특수한 Render함수가 필요하였으며,
또한 좌우반전된 이미지를 렌더하기 위해서도 Buffer에 값을 직접 찍어야하는 DirectDraw 특성상 이에 맞는 Render함수제작이 필요하였습니다.


1) CalcSpriteClipArea 함수
아래 3가지를 계산해줌

 - SpriteSheet 내의 로컬 좌표계 상에서의 좌표
 - 실제 화면(BackBuffer, Screen 좌표계) 상에서의 좌표
 - Screen 좌표계 범위를 벗어남에 따라 Clipping이 되는데, Clipping 이후 그려야할 Rect의 Width와 Height 


 2) DrawSprite 함수
 srcRect값을 기반으로 SpriteSheet 내에서 그려야할 Sprite이미지를 Render하는 함수
 구체적인 설명은 주석으로 적어둠


 3) DrawSpriteFlip 함수
 DrawSprite 함수에서 좌우반전된 이미지를 그려주는 함수
 구체적인 설명은 주석으로 적어둠

*/



BOOL CDDrawDevice::CalcSpriteClipArea(
	INT_VECTOR2* pivOutSrcStart,      // 계산된 소스 시작 좌표 (스프라이트 시트 내)
	INT_VECTOR2* pivOutDestStart,     // 계산된 화면상의 시작 좌표
	INT_VECTOR2* pivOutDestSize,      // 실제 렌더링할 영역의 크기
	const RECT& srcRect,              // 스프라이트 시트 내에서의 렌더링 영역 (예: 프레임 영역)
	const INT_VECTOR2* pDestPos,      // 화면상의 렌더링 시작 좌표 ({screenX, screenY})
	const INT_VECTOR2* pBufferSize)   // 화면(버퍼) 크기 ({width, height})
{

	// 1. srcRect를 기반으로 원하는 스프라이트 영역 크기 계산
	INT_VECTOR2 desiredSize;
	desiredSize.x = srcRect.right - srcRect.left;
	desiredSize.y = srcRect.bottom - srcRect.top;

	// 2. 화면상의 렌더링 영역 클리핑: destPos와 desiredSize를 이용하여 실제 그릴 영역 결정
	int destStartX = std::max<int>(pDestPos->x, 0);
	int destStartY = std::max<int>(pDestPos->y, 0);
	int destEndX = std::min<int>(pDestPos->x + desiredSize.x, pBufferSize->x);
	int destEndY = std::min<int>(pDestPos->y + desiredSize.y, pBufferSize->y);

	int clippedWidth = destEndX - destStartX;
	int clippedHeight = destEndY - destStartY;

	if (clippedWidth <= 0 || clippedHeight <= 0)
		return FALSE;			// 렌더링할 영역이 없음

	// 3. 스프라이트 시트 내 소스 시작 좌표 계산:
	// 화면상의 시작 좌표와 원래 destPos의 차이를 srcRect의 좌측/상단에 더함
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

	// 화면의 너비 (출력 대상)
	int screenWidth = static_cast<int>(m_dwWidth);

	// 스프라이트 프레임의 크기 계산 (srcRect 기준)
	int frameWidth = srcRect.right - srcRect.left;
	int frameHeight = srcRect.bottom - srcRect.top;

	// 스프라이트 시트 전체 이미지의 크기
	/*
	int imageWidth = static_cast<int>(pImgData->GetWidth());
	int imageHeight = static_cast<int>(pImgData->GetHeight());
	*/

	// 렌더링할 소스 시작 좌표 (스프라이트 시트 로컬 좌표계)
	INT_VECTOR2 srcStart = { srcRect.left, srcRect.top };

	// 화면상의 시작 좌표 (스크린 좌표계)
	INT_VECTOR2 destStart = { screenX, screenY };

	// 복사할 영역의 크기 초기값은 스프라이트 프레임의 크기로 설정
	INT_VECTOR2 spriteSize = { frameWidth, frameHeight };
	INT_VECTOR2 clippedSize = {};

	// 대상 위치와 버퍼 크기
	INT_VECTOR2 destPos = { screenX, screenY };
	INT_VECTOR2 bufferSize = { screenWidth, static_cast<int>(m_dwHeight) };

	// CalcClipArea() 함수는 destStart와 spriteSize를 기반으로
	// 실제로 화면에 그려질 영역(클립된 영역)을 계산
	if (!CalcSpriteClipArea(&srcStart, &destStart, &clippedSize, srcRect, &destPos, &bufferSize))
		goto lb_return;


	// pImgData에서 srcStart.y 행의 압축된 데이터를 가져오기
	const COMPRESSED_LINE* lineDesc = pImgData->GetCompressedImage(srcStart.y);

	// 대상 버퍼의 첫 행 포인터 (destStart.y 행부터 시작)
	char* destLinePtr = m_pWriteBuffer + (destStart.y) * m_dwWriteBufferPitch;

	// 클립된 영역의 높이만큼 반복
	for (int y = 0; y < clippedSize.y; y++)
	{
		// 각 줄의 스트림 데이터를 순회
		for (DWORD i = 0; i < lineDesc->dwStreamNum; i++)
		{
			PIXEL_STREAM* pStream = lineDesc->pPixelStream + i;
			DWORD pixelColor = pStream->dwPixel;
			int pixelCount = static_cast<int>(pStream->wPixelNum);

		


			/*
			이는 0번압축에서 전체 Sprite이미지 중 Rect범위안에 해당하는 이미지만 Render하기 위한 Case들임
			만약 아래 Case들을 고려하지 않으면 스프라이트 시트 전체가 렌더링됨
			
			pStream->wPosX 는 전체 이미지의 로컬좌표계상에서의 연속된 픽셀이 시작되는 X좌표이라는 점을 고려해야함
			즉 {wPosX, wPosX + pixelCount} 만큼이 현재 Line에서 연속된 구간이라는것
			기존 좌우 Screen범위에 맞춰 그려야할 수를 조절하는 로직을 수정하던가, 그 전에 작업을 추가해서 처리해야함

			우선 현재 srcRect 범위 안에 맞춰 시작 X를 정해야할 것 같음
			즉 로컬좌표계 계산부터 수행하기

			Case1)   wPosX, wPosX + pixelCount가 Rect범위 좌측을 넘어갔을 시 당연히 Render할 필요가 없음
			Case2)   wPosX는 넘어가고 wPosX + pixelCount는 Rect범위 안쪽에 있을 시
					   이 경우, 시작 좌표는 Rect.left 가 될테고, PixelCount수는 (wPosX + pixelCount - Rect.left) 이게 될 것

			Case3)	wPosX, wPosX + pixelCount가 Rect범위 안쪽일 시 그대로 사용
			Case4)	wPosX는 Rect안쪽, wPosX + pixelCount는 Rect바깥쪽
					    이 경우, 시작좌표는 wPosX, pixelCount는 (Rect.Right - wPosX)

			Case5)	wPosX, wPosX + pixelCount 둘 다 Rect 바깥쪽일 시 Render할 필요가 없음

			이러면 Rect안에서 그려질 localRenderStartX, pixelCount를 구해낸 것
			이를 기반으로 screenRenderX = screenX + localRenderStartX
			이런식으로 두면 될 거 같음

			이후 기존 screenRenderX가 화면 좌측경계, 우측경계를 벗어낫을 시 처리도 수행하면 될 거 같음

			추가로 Rect기준 Local 좌표계로 생각했을 때 (Rect.left를 빼면됨)
			즉 (srcStart.x - srcRect.left) 값이 양수인 경우 화면경계 좌측에서 clipping이 일어난 것이므로 이에 대한 처리도 추가함
			
			*/

			
			// Rect 기준 Local 좌표계로 변경해서 생각하기 (srcRect.left를 빼주는 것)
			// 이 때 (srcStart.x - srcRect.left) 가 양수이면 clipping이 일어난 것이므로, 그 부분을 고려하여 localX와 frameWidth를 조절
			// localX = (wPosX - srcRect.left) - (srcStart.x - srcRect.left) = wPosX - srcStart.x
			int localX = static_cast<int>(pStream->wPosX) - srcStart.x;			// 위의 수식에 의해 srcRect.left는 소거됨
			frameWidth = srcRect.right - srcStart.x;
			

			// Case1, Case5: srcRect 범위를 벗어난 경우
			if (localX + pixelCount <= 0 || localX >= frameWidth)
				continue;

			
			// Case2: 왼쪽이 벗어난 경우
			if (localX < 0) {
				pixelCount += localX; // localX는 음수
				localX = 0;
			}

			// Case4: 오른쪽을 벗어난 경우
			if (localX + pixelCount > frameWidth) {
				pixelCount = frameWidth - localX;
			}


			// 최종적으로 화면상의 x 좌표 = destStart.x + localX
			int drawX = destStart.x + localX;


			// 화면 좌측 경계를 벗어나면 조정
			
			if (drawX < 0)
			{
				pixelCount += drawX;
				drawX = 0;
			}
			

			// 화면 우측 경계를 벗어나면 조정
			if (drawX + pixelCount > screenWidth)
			{
				pixelCount = screenWidth - drawX;
			}


			// 대상 버퍼 내 해당 픽셀 위치 계산 (픽셀당 4Byte)
			char* destPixelPtr = destLinePtr + (drawX * 4);

			// 해당 스트림의 픽셀들을 복사
			for (int x = 0; x < pixelCount; x++)
			{
				*(DWORD*)destPixelPtr = pixelColor;
				destPixelPtr += 4;
			}
		}
		lineDesc++; // 다음 행의 압축 데이터
		destLinePtr += m_dwWriteBufferPitch; // 대상 버퍼의 다음 행 포인터
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

    // 화면의 너비 (출력 대상)
    int screenWidth = static_cast<int>(m_dwWidth);

    // 스프라이트 프레임의 크기 (srcRect 기준)
    int frameWidth = srcRect.right - srcRect.left;
    int frameHeight = srcRect.bottom - srcRect.top;

    // 렌더링할 소스 시작 좌표 (스프라이트 시트 로컬 좌표계)
    INT_VECTOR2 srcStart = { srcRect.left, srcRect.top };

    // 화면상의 시작 좌표 (스크린 좌표계)
    INT_VECTOR2 destStart = { screenX, screenY };

    // 복사할 영역의 크기는 스프라이트 프레임의 크기로 초기화
    INT_VECTOR2 spriteSize = { frameWidth, frameHeight };
    INT_VECTOR2 clippedSize = {};

    // 대상 위치와 버퍼 크기
    INT_VECTOR2 destPos = { screenX, screenY };
    INT_VECTOR2 bufferSize = { screenWidth, static_cast<int>(m_dwHeight) };

    // CalcClipArea() 함수는 destStart와 spriteSize를 기반으로 실제 화면에 그려질 영역(클립된 영역)을 계산
    if (!CalcSpriteClipArea(&srcStart, &destStart, &clippedSize, srcRect, &destPos, &bufferSize))
        goto lb_return;

    // pImgData에서 srcStart.y 행의 압축 데이터를 가져옴
    const COMPRESSED_LINE* lineDesc = pImgData->GetCompressedImage(srcStart.y);

    // 대상 버퍼의 첫 행 포인터 (destStart.y 행부터 시작)
    char* destLinePtr = m_pWriteBuffer + (destStart.y) * m_dwWriteBufferPitch;

    // 클립된 영역의 높이만큼 반복
    for (int y = 0; y < clippedSize.y; y++)
    {
        // 각 행의 압축된 스트림 데이터를 순회
        for (DWORD i = 0; i < lineDesc->dwStreamNum; i++)
        {
            PIXEL_STREAM* pStream = lineDesc->pPixelStream + i;
            DWORD pixelColor = pStream->dwPixel;
            int pixelCount = static_cast<int>(pStream->wPixelNum);

			// Rect 좌상단을 기준으로 하는 로컬좌표계르 기준으로 생각해야 로직짜기가 편함
			// 따라서 srcRect.left를 빼줘야함
			int localX = static_cast<int>(pStream->wPosX) - srcRect.left;
			// frameWidth는 srcRect의 너비
			frameWidth = srcRect.right - srcRect.left;

            // 현재 스트림이 srcRect 범위를 벗어나면 건너뜀
            if (localX + pixelCount <= 0 || localX >= frameWidth)
                continue;

            // Case2: 왼쪽을 벗어난 경우
            if (localX < 0) {
                pixelCount += localX; // localX는 음수
                localX = 0;
            }

            // Case4: 오른쪽을 벗어난 경우
            if (localX + pixelCount > frameWidth) {
                pixelCount = frameWidth - localX;
            }

            // 좌우 반전 처리:
            // 원래 좌우 복사는 destStart.x + localX로 계산하지만,
            // 반전된 경우엔 해당 프레임의 오른쪽 끝에서부터 localX만큼 떨어진 위치로 복사하고,
            // pixelCount만큼 픽셀을 채우므로,
            // drawX = destStart.x + (frameWidth - localX - pixelCount) 가 되어야한다.
            int drawX = destStart.x + (frameWidth - localX - pixelCount);
			
			// 이번에는 여기서 최종 좌표위치를 왼쪽에서 Clipping 된 만큼 옮겨줘야함
			// (srcStart.x - srcRect.left) 가 양수라면 왼쪽에서 Clippiing이 일어난 상황

			drawX -= (srcStart.x - srcRect.left);

            // 화면 좌측 경계 클리핑
            if (drawX < 0)
            {
                pixelCount += drawX;
                drawX = 0;
            }

            // 화면 우측 경계 클리핑
            if (drawX + pixelCount > screenWidth)
            {
                pixelCount = screenWidth - drawX;
            }

            // 대상 버퍼 내 해당 픽셀 위치 계산 (픽셀당 4Byte)
            // (반전된 이미지는 오른쪽에서부터 채워지므로, 여기서는 일반적인 순서로 복사해도 동일한 색상 반복 복사 시 문제없음)
            char* destPixelPtr = destLinePtr + (drawX * 4);

            // 동일 색상의 픽셀들을 pixelCount만큼 복사
            for (int x = 0; x < pixelCount; x++)
            {
                *(DWORD*)destPixelPtr = pixelColor;
                destPixelPtr += 4;
            }
        }
        lineDesc++; // 다음 행의 압축 데이터
        destLinePtr += m_dwWriteBufferPitch; // 대상 버퍼의 다음 행 포인터
    }
    bResult = TRUE;
lb_return:
    return bResult;
}




