/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   IEcoMemoryAllocator1
 * </сводка>
 *
 * <описание>
 *   Данный исходный код описывает реализацию интерфейсов IEcoMemoryAllocator1
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2016 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#include "IEcoSystem1.h"
#include "CEcoMemoryManager1Lab.h"


/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция QueryInterface для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_QueryInterface(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoMemoryManager1) ) {
        *ppv = &pCMe->m_pVTblIMgr;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoMemoryAllocator1) ) {
        *ppv = &pCMe->m_pVTblIMem;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoVirtualMemory1) ) {
        *ppv = &pCMe->m_pVTblIVirtual;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIMgr;
        pCMe->m_pVTblIMgr->AddRef((IEcoMemoryManager1*)pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }

    return ERR_ECO_SUCCESES;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_AddRef(/* in */ IEcoMemoryAllocator1Ptr_t me) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 ) {
        return -1;
    }

    return atomicincrement_int32_t(&pCMe->m_cRef);
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoMemoryAllocator1
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Release(/* in */ IEcoMemoryAllocator1Ptr_t me) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));

    if (me == 0 ) {
        return -1;
    }

    /* Уменьшение счетчика ссылок на компонент */
    atomicdecrement_int32_t(&pCMe->m_cRef);

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if ( pCMe->m_cRef == 0 ) {
        deleteCEcoMemoryManager1Lab_623E1838((IEcoMemoryManager1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Alloc
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Alloc(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ uint32_t size) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));
    MemoryBlock* head = 0;
    MemoryBlock* start = 0;
    MemoryBlock* cur = 0;
    MemoryBlock* chosen = 0;

    if (me == 0 || size == 0) {
        return 0;
    }

    if (pCMe->m_pHeapStart == 0) {
        return 0;
    }

    /* выравнивание под размер указателя */
    size = (size + sizeof(voidptr_t) - 1) & ~(sizeof(voidptr_t) - 1);

    head = (MemoryBlock*)pCMe->m_pHeapStart;

    /* если курсор ещё не инициализирован, то начинаем с начала */
    start = (pCMe->m_pNextFitCursor != 0) ? pCMe->m_pNextFitCursor : head;
    cur = start;

    do {
        if (cur->isFree && cur->payload >= size) {
            chosen = cur;
            break;
        }

        cur = (cur->next != 0) ? cur->next : head;

    } while (cur != start);

    if (chosen == 0) {
        /* подходящего блока нет */
        return 0;
    }

    if (chosen->payload >= size + sizeof(MemoryBlock) + sizeof(voidptr_t)) {
        MemoryBlock* tail =
            (MemoryBlock*)((char_t*)chosen + sizeof(MemoryBlock) + size);

        tail->payload = chosen->payload - size - sizeof(MemoryBlock);
        tail->isFree = 1;

        tail->prev = chosen;
        tail->next = chosen->next;

        if (tail->next != 0) {
            tail->next->prev = tail;
        }

        chosen->next = tail;
        chosen->payload = size;
    }

    chosen->isFree = 0;

    pCMe->m_iAllocSize += chosen->payload;
    pCMe->m_iBlockNum  += 1;

    pCMe->m_pNextFitCursor = (chosen->next != 0) ? chosen->next : head;

    return (void*)((char_t*)chosen + sizeof(MemoryBlock));
}

/*
 *
 * <сводка>
 *   Функция Free
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Free(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));
    MemoryBlock* block = 0;

    if (me == 0 || pv == 0) {
        return;
    }

    if (pCMe->m_pHeapStart == 0) {
        return;
    }

    /* получаем заголовок блока */
    block = (MemoryBlock*)((char_t*)pv - sizeof(MemoryBlock));

    /* защита от double free */
    if (block->isFree) {
        return;
    }

    /* помечаем блок свободным */
    block->isFree = 1;

    if (pCMe->m_iAllocSize >= block->payload) {
        pCMe->m_iAllocSize -= block->payload;
    }

    if (pCMe->m_iBlockNum > 0) {
        pCMe->m_iBlockNum -= 1;
    }

    if (block->next != 0 && (block->next->isFree)) {
        MemoryBlock* next = block->next;

        if (pCMe->m_pNextFitCursor == next) {
            pCMe->m_pNextFitCursor = block;
        }

        block->payload += sizeof(MemoryBlock) + next->payload;
        block->next = next->next;

        if (block->next != 0) {
            block->next->prev = block;
        }
    }

    if (block->prev != 0 && (block->prev->isFree)) {
        MemoryBlock* prev = block->prev;

        if (pCMe->m_pNextFitCursor == block) {
            pCMe->m_pNextFitCursor = prev;
        }

        prev->payload += sizeof(MemoryBlock) + block->payload;
        prev->next = block->next;

        if (prev->next != 0) {
            prev->next->prev = prev;
        }

        block = prev;
    }

    if (pCMe->m_pNextFitCursor == 0) {
        pCMe->m_pNextFitCursor = (MemoryBlock*)pCMe->m_pHeapStart;
    }
}

/*
 *
 * <сводка>
 *   Функция Copy
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Copy(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pvDst, /* in */ voidptr_t pvSrc, /* in */ uint32_t size) {
    uint8_t* d = 0;
    const uint8_t* s = 0;
    uint32_t i = 0;

    if (pvDst == 0 || pvSrc == 0 || size == 0) {
        return pvDst;
    }

    d = (uint8_t*)pvDst;
    s = (const uint8_t*)pvSrc;

    for (i = 0; i < size; ++i) {
        d[i] = s[i];
    }

    return pvDst;
}

/*
 *
 * <сводка>
 *   Функция Realloc
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Realloc(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv, /* in */ uint32_t size) {
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)((uint64_t)me - sizeof(voidptr_t));
    MemoryBlock* oldHeader = 0;
    void* newPtr = 0;
    uint32_t copySize = 0;

    if (me == 0) {
        return 0;
    }

    if (pv == 0) {
        return CEcoMemoryAllocator1Lab_623E1838_Alloc(me, size);
    }

    if (size == 0) {
        CEcoMemoryAllocator1Lab_623E1838_Free(me, pv);
        return 0;
    }

    oldHeader = (MemoryBlock*)((char_t*)pv - sizeof(MemoryBlock));

    if (size <= oldHeader->payload) {
        return pv;
    }

    newPtr = CEcoMemoryAllocator1Lab_623E1838_Alloc(me, size);
    if (newPtr == 0) {
        return 0;
    }

    copySize = oldHeader->payload;
    CEcoMemoryAllocator1Lab_623E1838_Copy(me, newPtr, pv, copySize);

    CEcoMemoryAllocator1Lab_623E1838_Free(me, pv);

    return newPtr;
}

/*
 *
 * <сводка>
 *   Функция Fill
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void* ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Fill(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pvDst, /* in */ char_t Fill, /* in */ uint32_t size) {
    uint8_t* d = 0;
    uint32_t i = 0;

    if (pvDst == 0 || size == 0) {
        return pvDst;
    }

    d = (uint8_t*)pvDst;

    for (i = 0; i < size; ++i) {
        d[i] = (uint8_t)Fill;
    }

    return pvDst;
}

/*
 *
 * <сводка>
 *   Функция Compare
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoMemoryAllocator1Lab_623E1838_Compare(/* in */ IEcoMemoryAllocator1Ptr_t me, /* in */ voidptr_t pv1, /* in */ voidptr_t pv2, /* in */ uint32_t size) {
    const uint8_t* a = 0;
    const uint8_t* b = 0;
    uint32_t i = 0;

    if (pv1 == 0 || pv2 == 0) {
        return -1;
    }

    if (size == 0) {
        return 0;
    }

    a = (const uint8_t*)pv1;
    b = (const uint8_t*)pv2;

    for (i = 0; i < size; ++i) {
        if (a[i] != b[i]) {
            return (a[i] < b[i]) ? -1 : 1;
        }
    }

    return 0;
}
