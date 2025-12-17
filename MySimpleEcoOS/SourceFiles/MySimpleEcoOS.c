/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoVFB1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoMemoryManager1Lab.h"
#include "IdEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "IdEcoIPCCMailbox1.h"
#include "IdEcoVFB1.h"
#include "IEcoVBIOS1Video.h"

/* Начало свободного участка памяти */
extern char_t __heap_start__;

/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoMemoryManager1* pIMemMgr = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    /* Указатель на интерфейс для работы c буфером кадров видеоустройства */
    IEcoVFB1* pIVFB = 0;
    ECO_VFB_1_SCREEN_MODE xScreenMode = {0};
    IEcoVBIOS1Video* pIVideo = 0;

    char_t* a = 0;
    char_t* b = 0;
    char_t* c = 0;
    char_t* r = 0;

    char_t* ptrPart1 = 0;
    char_t* ptrPart2 = 0;
    char_t* ptrMerged = 0;
    char_t* ptrPart1Saved = 0;

    char_t* ptrs[20] = {0};
    int16_t i = 0;
    int16_t passed = 1;
    char_t* pBig = 0;
    char_t* pMid1 = 0;
    char_t* pMid2 = 0;

    /* Создание экземпляра интерфейсной шины */
    result = GetIEcoComponentFactoryPtr_00000000000000000000000042757331->pVTbl->Alloc(GetIEcoComponentFactoryPtr_00000000000000000000000042757331, 0, 0, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    /* Проверка */
    if (result != 0 && pIBus == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с памятью */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMemoryManager1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_0000000000000000000000004D656D31);
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMemoryManager1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_81589BFED0B84B1194524BEE623E1838);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с ящиком прошивки */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoIPCCMailbox1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_F10BC39A4F2143CF8A1E104650A2C302);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Запрос расширения интерфейсной шины */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        /* Установка расширения менаджера памяти */
        pIMemExt->pVTbl->set_Manager(pIMemExt, &CID_EcoMemoryManager1Lab);
        /* Установка разрешения расширения пула */
        pIMemExt->pVTbl->set_ExpandPool(pIMemExt, 1);
    }

    /* Получение интерфейса управления памятью */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1Lab, 0, &IID_IEcoMemoryManager1, (void**) &pIMemMgr);
    if (result != 0 || pIMemMgr == 0) {
        /* Возврат в случае ошибки */
        return result;
    }

    /* Выделение области памяти 512 КБ */
    pIMemMgr->pVTbl->Init(pIMemMgr, &__heap_start__, 0x080000);

    /* Регистрация статического компонента для работы с VBF */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoVFB1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_939B1DCDB6404F7D9C072291AF252188);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Получение интерфейса для работы с памятью */
    result = pIMemMgr->pVTbl->QueryInterface(pIMemMgr, &IID_IEcoMemoryAllocator1, (void**)&pIMem);
    if (result != 0 || pIMem == 0) {
        goto Release;
    }

    /* Получение интерфейса для работы с видео сервисами VBF */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoVFB1, 0, &IID_IEcoVFB1, (void**) &pIVFB);
    /* Проверка */
    if (result != 0 || pIVFB == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Получение информации о текущем режиме экрана */
    result = pIVFB->pVTbl->get_Mode(pIVFB, &xScreenMode);
    pIVFB->pVTbl->Create(pIVFB, 0, 0, xScreenMode.Width, xScreenMode.Height);
    result = pIVFB->pVTbl->QueryInterface(pIVFB, &IID_IEcoVBIOS1Video, (void**) &pIVideo);

    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, "TEST 1: Next Fit Selection", 26);
    
    a = (char_t*)pIMem->pVTbl->Alloc(pIMem, 128);
    b = (char_t*)pIMem->pVTbl->Alloc(pIMem, 128);
    c = (char_t*)pIMem->pVTbl->Alloc(pIMem, 128);
    
    if (!a || !b || !c) {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 1, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "FAIL: allocation setup error", 28);
        goto Release;
    }
    
    pIMem->pVTbl->Free(pIMem, a);
    r = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);
    
    if (r && r != a) {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 1, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "PASS: early free block skipped", 30);
    } else {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 1, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "FAIL: allocation started from head", 34);
    }
    
    if (r) pIMem->pVTbl->Free(pIMem, r);
    pIMem->pVTbl->Free(pIMem, b);
    pIMem->pVTbl->Free(pIMem, c);
    
    
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 3, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, "TEST 2: Coalescing", 18);
    
    ptrPart1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 200);
    ptrPart2 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 200);
    ptrPart1Saved = ptrPart1;
    
    pIMem->pVTbl->Free(pIMem, ptrPart1);
    pIMem->pVTbl->Free(pIMem, ptrPart2);
    
    ptrMerged = (char_t*)pIMem->pVTbl->Alloc(pIMem, 350);
    
    if (ptrMerged && ptrMerged == ptrPart1Saved) {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 4, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "PASS: adjacent blocks merged", 28);
        pIMem->pVTbl->Free(pIMem, ptrMerged);
    } else {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 4, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "FAIL: blocks not merged", 23);
    }
    
    
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 6, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, "TEST 3: Fragmentated allocations stress test", 44);
    
    for (i = 0; i < 20; ++i) {
        ptrs[i] = (char_t*)pIMem->pVTbl->Alloc(pIMem, 128);
        if (!ptrs[i]) passed = 0;
        else pIMem->pVTbl->Fill(pIMem, ptrs[i], (char_t)i, 128);
    }
    
    if (passed)
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 7, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "PASS: alloc many blocks", 23);
    else
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 7, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "FAIL: alloc many blocks", 23);
    
    for (i = 0; i < 20; i += 2) {
        if (ptrs[i]) pIMem->pVTbl->Free(pIMem, ptrs[i]);
    }
    
    pBig = (char_t*)pIMem->pVTbl->Alloc(pIMem, 128);
    if (pBig) {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 8, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "PASS: alloc after fragmentation", 31);
        pIMem->pVTbl->Free(pIMem, pBig);
    }
    
    for (i = 1; i < 20; i += 2) {
        if (ptrs[i]) pIMem->pVTbl->Free(pIMem, ptrs[i]);
    }
    
    
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 10, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, "TEST 4: Realloc", 15);
    
    pMid1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 32);
    pIMem->pVTbl->Fill(pIMem, pMid1, 'X', 32);
    
    pBig = (char_t*)pIMem->pVTbl->Realloc(pIMem, pMid1, 128);
    if (pBig && *pBig == 'X') {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 11, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "PASS: realloc expand", 20);
        pMid2 = (char_t*)pIMem->pVTbl->Realloc(pIMem, pBig, 16);
        if (pMid2 && *pMid2 == 'X') {
            pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 12, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "PASS: realloc shrink", 20);
            pIMem->pVTbl->Free(pIMem, pMid2);
        }
    }
    
    
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 14, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, "TEST 5: Cursor Stability", 24);
    
    char_t* s1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 128);
    char_t* s2 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 128);
    char_t* s3 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 128);
    
    pIMem->pVTbl->Free(pIMem, s1);
    pIMem->pVTbl->Free(pIMem, s2);
    
    char_t* s4 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 64);
    
    if (s4 != s1) {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 15, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "PASS: cursor position preserved", 31);
    } else {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 15, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "FAIL: cursor reset detected", 27);
    }
    
    if (s3) {
        pIMem->pVTbl->Free(pIMem, s3);
    }
    if (s4) {
        pIMem->pVTbl->Free(pIMem, s4);
    }
    
    
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 17, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, "TEST 6: Double Free Safety", 26);
    
    char_t* df = (char_t*)pIMem->pVTbl->Alloc(pIMem, 128);
    pIMem->pVTbl->Free(pIMem, df);
    pIMem->pVTbl->Free(pIMem, df);
    
    char_t* df2 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 128);
    
    if (df2) {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 18, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "PASS: double free handled", 25);
        pIMem->pVTbl->Free(pIMem, df2);
    } else {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 18, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "FAIL: allocator state corrupted", 31);
    }
    
    
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 20, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, "Tests completed!", 16);

    while(1) {
        asm volatile ("NOP\n\t" ::: "memory");
    }

Release:

    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }

    if (pIMemMgr != 0) {
        pIMemMgr->pVTbl->Release(pIMemMgr);
    }

    /* Освобождение интерфейса VFB */
    if (pIVFB != 0) {
        pIVFB->pVTbl->Release(pIVFB);
    }

    /* Освобождение системного интерфейса */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}

