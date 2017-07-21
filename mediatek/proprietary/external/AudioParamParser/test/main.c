#include "AudioParamParser.h"
#include "AudioParamParserPriv.h"

#include <stdio.h>
#include <string.h>

#define BUF_SIZE 1024

void showParamValue(Param *param)
{
    switch (param->paramInfo->dataType)
    {
        case TYPE_STR:
        {
            char *value = (char *)param->data;
            printf("param name = %s, value = %s (type = %s)\n", param->name, value, paramDataTypeToStr(param->paramInfo->dataType));
            break;
        }
        case TYPE_INT:
        {
            int value = *(int *) param->data;
            printf("param name = %s, value = %d (type = %s)\n", param->name, value, paramDataTypeToStr(param->paramInfo->dataType));
            break;
        }
        case TYPE_UINT:
        {
            unsigned int value = *(unsigned int *) param->data;
            printf("param name = %s, value = %d (type = %s)\n", param->name, value, paramDataTypeToStr(param->paramInfo->dataType));
            break;
        }
        case TYPE_FLOAT:
        {
            float value = *(float *) param->data;
            /* JH's platform cannot show the float type, wei chiu could show the value by %f*/
            printf("param name = %s, value = %d (type = %s)\n", param->name, (int)value, paramDataTypeToStr(param->paramInfo->dataType));
            break;
        }
        case TYPE_BYTE_ARRAY:
        {
            char *byteArray = (char *)param->data;
            int arraySize = param->arraySize;
            int i = 0;
            printf("param name = %s (type = %s, size = %d)\n", param->name, paramDataTypeToStr(param->paramInfo->dataType), arraySize);
            for (i = 0; i < arraySize; i++)
            {
                printf("\tarray[%d] %d\n", i, byteArray[i]);
            }
            break;
        }
        case TYPE_UBYTE_ARRAY:
        {
            unsigned char *ubyteArray = (unsigned char *)param->data;
            int arraySize = param->arraySize;
            int i = 0;
            printf("param name = %s (type = %s, size = %d)\n", param->name, paramDataTypeToStr(param->paramInfo->dataType), arraySize);
            for (i = 0; i < arraySize; i++)
            {
                printf("\tarray[%d] 0x%x\n", i, ubyteArray[i]);
            }
            break;
        }
        case TYPE_SHORT_ARRAY:
        {
            short *shortArray = (short *)param->data;
            int arraySize = param->arraySize;
            int i = 0;
            printf("param name = %s (type = %s, size = %d)\n", param->name, paramDataTypeToStr(param->paramInfo->dataType), arraySize);
            for (i = 0; i < arraySize; i++)
            {
                printf("\tarray[%d] %d\n", i, shortArray[i]);
            }
            break;
        }
        case TYPE_USHORT_ARRAY:
        {
            unsigned short *ushortArray = (unsigned short *)param->data;
            int arraySize = param->arraySize;
            int i = 0;
            printf("param name = %s (type = %s, size = %d)\n", param->name, paramDataTypeToStr(param->paramInfo->dataType), arraySize);
            for (i = 0; i < arraySize; i++)
            {
                printf("\tarray[%d] 0x%x\n", i, ushortArray[i]);
            }
            break;
        }
        case TYPE_INT_ARRAY:
        {
            int *intArray = (int *)param->data;
            int arraySize = param->arraySize;
            int i = 0;
            printf("param name = %s (type = %s, size = %d)\n", param->name, paramDataTypeToStr(param->paramInfo->dataType), arraySize);
            for (i = 0; i < arraySize; i++)
            {
                printf("\tarray[%d] %d\n", i, intArray[i]);
            }
            break;
        }
        case TYPE_UINT_ARRAY:
        {
            unsigned int *uintArray = (unsigned int *)param->data;
            int arraySize = param->arraySize;
            int i = 0;
            printf("param name = %s (type = %s, size = %d)\n", param->name, paramDataTypeToStr(param->paramInfo->dataType), arraySize);
            for (i = 0; i < arraySize; i++)
            {
                printf("\tarray[%d] 0x%x\n", i, uintArray[i]);
            }
            break;
        }
        case TYPE_DOUBLE_ARRAY:
        {
            double *doubleArray = (double *)param->data;
            int arraySize = param->arraySize;
            int i = 0;
            printf("param name = %s (type = %s, size = %d)\n", param->name, paramDataTypeToStr(param->paramInfo->dataType), arraySize);
            for (i = 0; i < arraySize; i++)
            {
                printf("\tarray[%d] %f\n", i, doubleArray[i]);
            }
            break;
        }
        default:
            printf("param name = %s, value = 0x%p (type = %s)\n", param->name, param->data, paramDataTypeToStr(param->paramInfo->dataType));
    }
}

void showFieldValueOfParam(Param *param)
{
    // Enum all field value
    size_t i;
    ParamInfo *paramInfo = audioTypeGetParamInfoByName(param->paramUnit->audioType, param->name);
    unsigned int val;
    size_t numOfFieldInfo;

    printf("\t---------------------\n");
    numOfFieldInfo = paramInfoGetNumOfFieldInfo(paramInfo);
    for (i = 0; i < numOfFieldInfo; i++)
    {

        FieldInfo *fieldInfo = paramInfoGetFieldInfoByIndex(paramInfo, i);
        if (paramGetFieldVal(param, fieldInfo, &val) == APP_ERROR)
        {
            printf("Cannot get field value. (param name=%s, field name=%s)\n", param->name, fieldInfo->name);
            continue;
        }

        printf("\tfield[%lu] name = %s, value = 0x%u.\n", i, fieldInfo->name, val);
    }
}

void showFieldInfo(FieldInfo *fieldInfo)
{
    if (!fieldInfo)
    {
        return;
    }

    printf("FieldInfo name = %s, array_index = %lu, bit[%d,%d], check_list = %s\n", fieldInfo->name, fieldInfo->arrayIndex, fieldInfo->startBit, fieldInfo->endBit, fieldInfo->checkListStr);
}

void showParamInfo(ParamInfo *paramInfo)
{
    size_t i;
    size_t numOfFieldInfo = paramInfoGetNumOfFieldInfo(paramInfo);

    printf("ParamInfo name = %s, type = %s\n", paramInfo->name, paramDataTypeToStr(paramInfo->dataType));
    for (i = 0; i < numOfFieldInfo; i++)
    {
        FieldInfo *fieldInfo = paramInfoGetFieldInfoByIndex(paramInfo, i);
        if (fieldInfo)
        {
            printf("\t[%lu] ", i);
            showFieldInfo(fieldInfo);
        }
    }
}

#if 0
void showCategoryTypeList(AudioType *audioType)
{
    xmlNode *categoryTypeListNode, *categoryTypeNode, *categoryNode, *subCategoryNode;

    printf("\n====Dump \"%s\" AudioType CategoryList Info====\n", audioType->name);
    categoryTypeListNode = audioTypeGetCategoryTypeListNode(audioType);
    if (!categoryTypeListNode)
    {
        printf("No category type list node!\n");
        return;
    }

    categoryTypeNode = categoryTypeListNode->children;

    while ((categoryTypeNode = findXmlNodeByElemName(categoryTypeNode->next, ELEM_CATEGORY_TYPE)))
    {
        printf("CategoryType, wording = %s, name = %s\n", xmlNodeGetWording(categoryTypeNode), xmlNodeGetProp(categoryTypeNode, ATTRI_NAME));

        categoryNode = categoryTypeNode->children;
        for (categoryNode = categoryTypeNode->children; categoryNode; categoryNode = categoryNode->next)
        {
            if (!strcmp((char *)categoryNode->name, ELEM_CATEGORY))
            {
                // Show Category
                printf("\t%s wording = %s (name = %s)\n", categoryNode->name, xmlNodeGetWording(categoryNode), xmlNodeGetProp(categoryNode, ATTRI_NAME));
            }
            else if (!strcmp((char *)categoryNode->name, ELEM_CATEGORY_GROUP))
            {
                // Show CategoryGroup
                printf("\t%s wording = %s (name = %s)\n", categoryNode->name, xmlNodeGetWording(categoryNode), xmlNodeGetProp(categoryNode, ATTRI_NAME));

                // Show Category's sub category
                for (subCategoryNode = categoryNode->children; subCategoryNode; subCategoryNode = subCategoryNode->next)
                {
                    if (!strcmp((char *)subCategoryNode->name, ELEM_CATEGORY))
                    {
                        printf("\t\t%s wording = %s (name = %s)\n", subCategoryNode->name, xmlNodeGetWording(subCategoryNode), xmlNodeGetProp(subCategoryNode, ATTRI_NAME));
                    }
                }
            }
        }
    }
}
#else
void showCategoryTypeList(AudioType *audioType)
{
    size_t i, j, k;
    size_t numOfCategory;
    size_t numOfCategoryType = audioTypeGetNumOfCategoryType(audioType);

    printf("\n====%s AudioType's Category====\n\n", audioType->name);
    for (i = 0; i < numOfCategoryType; i++)
    {
        CategoryType *categoryType = audioTypeGetCategoryTypeByIndex(audioType, i);

        /* Show CategoryGroup part */
        size_t numOfCategoryGroup = categoryTypeGetNumOfCategoryGroup(categoryType);
        printf("CategoryType["APP_SIZE_T_FT"] name = %s wording = %s %s\n", i, categoryType->name, categoryType->wording, categoryType->visible ? "" : "visible = 0");
        for (j = 0; j < numOfCategoryGroup; j++)
        {
            /* Show CategoryGroup's category */
            CategoryGroup *categoryGroup = categoryTypeGetCategoryGroupByIndex(categoryType, j);
            size_t numOfCategory = categoryGroupGetNumOfCategory(categoryGroup);
            printf("\tCategoryGroup["APP_SIZE_T_FT"] name = %s wording = %s %s\n", j, categoryGroup->name, categoryGroup->wording, categoryGroup->visible ? "" : "visible = 0");
            for (k = 0; k < numOfCategory; k++)
            {
                Category *category = categoryGroupGetCategoryByIndex(categoryGroup, k);
                printf("\t\tCategory["APP_SIZE_T_FT"] name = %s wording = %s %s\n", k , category->name, category->wording, category->visible ? "" : "visible = 0");
            }
        }

        /* Show CategoryType's category */
        numOfCategory = categoryTypeGetNumOfCategory(categoryType);
        for (k = 0; k < numOfCategory; k++)
        {
            Category *category = categoryTypeGetCategoryByIndex(categoryType, k);
            printf("\tCategory["APP_SIZE_T_FT"] name = %s wording = %s %s\n", k , category->name, category->wording, category->visible ? "" : "visible = 0");
        }
    }
}
#endif

void showParamFieldInfo(AudioType *audioType)
{
    int i;
    int numOfParamInfo;
    ParamInfo *paramInfo;

    /* Enum all param & it's field information */
    numOfParamInfo = audioTypeGetNumOfParamInfo(audioType);
    printf("\n====%s AudioType's param field info====\n\n", audioType->name);
    for (i = 0; i < numOfParamInfo; i++)
    {
        paramInfo = audioTypeGetParamInfoByIndex(audioType, i);
        printf("[%d] ", i);
        showParamInfo(paramInfo);
    }
}

void showParamTreeViewInfo(AudioType *audioType)
{
    printf("\n====%s AudioType's param tree view info====\n\n", audioType->name);
    if (!audioType->paramTreeView)
    {
        printf("No definition!\n");
        return;
    }

    printf("ParamTreeView version(%d.%d)\n", audioType->paramTreeView->verMaj, audioType->paramTreeView->verMin);
    if (audioType->paramTreeView->treeRootHash)
    {
        TreeRoot *treeRoot;
        Feature *feature;
        FeatureField *featureField;
        CategoryPath *categoryPath;
        size_t i = 0;
        /* Enum all TreeRoot */
        for (treeRoot = audioType->paramTreeView->treeRootHash; treeRoot ; treeRoot = treeRoot->hh.next)
        {

            printf("+TreeRoot[%d] name = %s\n", i++, treeRoot->name);
            if (treeRoot->switchFieldInfo)
            {
                printf("  +switch (audio_type = %s, param = %s, field = %s)\n",
                       treeRoot->switchFieldInfo ? treeRoot->switchFieldInfo->paramInfo->audioType->name : "",
                       treeRoot->switchFieldInfo ? treeRoot->switchFieldInfo->paramInfo->name : "",
                       treeRoot->switchFieldInfo ? treeRoot->switchFieldInfo->name : "");
            }
            else
            {
                printf("  +no switch\n");
            }

            /* Enum all Feature */
            for (feature = treeRoot->featureHash; feature ; feature = feature->hh.next)
            {
                printf("  +Feature name = %s, feature_option = %s(val = %s), switch (audio_type = %s, param = %s, field = %s)\n", feature->name,
                       feature->featureOption,
                       appHandleGetFeatureOptionValue(audioType->appHandle, feature->featureOption),
                       feature->switchFieldInfo ? feature->switchFieldInfo->paramInfo->audioType->name : "null",
                       feature->switchFieldInfo ? feature->switchFieldInfo->paramInfo->name : "null",
                       feature->switchFieldInfo ? feature->switchFieldInfo->name : "null");

                /* Enum all field */
                for (featureField = feature->featureFieldHash; featureField ; featureField = featureField->hh.next)
                {
                    printf("    +Field audio_type = %s, param = %s, name = %s\n",
                           featureField->fieldInfo->paramInfo->audioType->name,
                           featureField->fieldInfo->paramInfo->name,
                           featureField->fieldInfo->name);
                }

                /* Enum all category path */
                for (categoryPath = feature->categoryPathHash; categoryPath ; categoryPath = categoryPath->hh.next)
                {
                    printf("    +CategoryPath path = %s\n", categoryPath->path);
                }
            }
        }
    }
}

void showParamUnit(AudioType *audioType, const char *categoryPath)
{
    int i;
    int numOfParam;
    Param *param;
    ParamUnit *paramUnit = audioTypeGetParamUnit(audioType, categoryPath);
    if (paramUnit == NULL)
    {
        printf("Cannot find ParamUnit.\n");
        return;
    }


    /* Example: provide retrieve all param ways */
    numOfParam = paramUnitGetNumOfParam(paramUnit);
    printf("\n\n====Query all param unit's param (param unit id = %d)====\n", paramUnit->paramId);
    for (i = 0; i < numOfParam; i++)
    {
        /* Show param info */
        param = paramUnitGetParamByIndex(paramUnit, i);
        printf("[%d] ", i);
        showParamValue(param);

        /* Show field info */
        showFieldValueOfParam(param);
    }

    /* Example: retrieve param by name */
    printf("\n\n====Query specific param (name = %s)====\n", "speech_mode_para");
    param = paramUnitGetParamByName(paramUnit, "speech_mode_para");
    if (param)
    {
        showParamValue(param);
    }
}

void showAllAudioType(AppHandle *appHandle)
{
    size_t i;
    printf("\n==== List All Audio Type ===\n\n");
    for (i = 0; i < appHandleGetNumOfAudioType(appHandle); i++)
    {
        AudioType *audioType = appHandleGetAudioTypeByIndex(appHandle, i);

        /* XML Version check for AudioTuningTool */
        if (!audioTypeIsTuningToolSupportedXmlVer(audioType))
        {
            printf("Error: %s AudioType's XML version is newer than tuning tool supported ver. (ParamUnitDesc ver (%d,%d), AudioParam ver (%d,%d))\n", audioType->name, audioType->paramUnitDescVerMaj, audioType->paramUnitDescVerMin, audioType->audioParamVerMaj, audioType->audioParamVerMin);
            continue;
        }

        // Tuning tool support backward compatible, need to know the XML version
        printf("AudioType[%lu] : %s (tab name = %s, ParamUnitDesc ver = %d.%d, AudioParam ver = %d.%d)\n", i, audioType->name, audioType->tabName, audioType->paramUnitDescVerMaj, audioType->paramUnitDescVerMin, audioType->audioParamVerMaj, audioType->audioParamVerMin);
    }
}

void showAllCategoryInformation(AppHandle *appHandle)
{
    size_t i;
    for (i = 0; i < appHandleGetNumOfAudioType(appHandle); i++)
    {
        AudioType *audioType = appHandleGetAudioTypeByIndex(appHandle, i);

        /* Example to retrieve category info */
        showCategoryTypeList(audioType);
    }
}

void showAllParamFieldInformation(AppHandle *appHandle)
{
    size_t i;
    for (i = 0; i < appHandleGetNumOfAudioType(appHandle); i++)
    {
        AudioType *audioType = appHandleGetAudioTypeByIndex(appHandle, i);

        /* Example to retrieve ParamInfo */
        showParamFieldInfo(audioType);
    }
}

void showAllParamTreeInformation(AppHandle *appHandle)
{
    size_t i;
    for (i = 0; i < appHandleGetNumOfAudioType(appHandle); i++)
    {
        AudioType *audioType = appHandleGetAudioTypeByIndex(appHandle, i);

        /* Example to retrieve ParamInfo */
        showParamTreeViewInfo(audioType);
    }
}

void showFeatureOptions(AppHandle *appHandle)
{
    size_t i;
    printf("\n===== Feature Option =====\n");
    for (i = 0; i < appHandleGetNumOfFeatureOption(appHandle); i++)
    {
        FeatureOption *featureOption = appHandleGetFeatureOptionByIndex(appHandle, i);
        printf("[%lu] %s = \"%s\" (enabled = %d)\n", i, featureOption->name, featureOption->value, appHandleIsFeatureOptionEnabled(appHandle, featureOption->name));
    }
    printf("============================\n");
}

void processTreeRootNode(xmlNode *node, int level, TreeRoot *treeRoot, const char *categoryPath)
{
    int isFeatureNode = 0;

    if (!node)
    {
        return;
    }

    if (node->type == XML_ELEMENT_NODE)
    {
        /* Show indent first */
        int i = 0;
        for (i = 0; i < level; i++)
        {
            printf("  ");
        }

        /* Process each element */
        if (!strcmp(node->name, ELEM_TREE_ROOT))
        {
            printf("+<TreeRoot name = %s>\n", xmlNodeGetProp(node, ATTRI_NAME));
        }
        else if (!strcmp(node->name, ELEM_SHEET))
        {
            /* Show sheet node */
            if (treeRoot->switchFieldInfo)
            {
                int fieldVal = 0;
                int onVal = 0;

                ParamUnit *paramUnit = audioTypeGetParamUnit(treeRoot->paramTreeView->audioType, categoryPath);
                paramUnitGetFieldVal(paramUnit, treeRoot->switchFieldInfo->paramInfo->name, treeRoot->switchFieldInfo->name, &fieldVal);

                if ((fieldInfoGetCheckListValue(treeRoot->switchFieldInfo, "on", &onVal) == APP_NO_ERROR)
                    && onVal == fieldVal)
                {
                    printf("+<\"check\" Sheet %s>\n", categoryPath);    // checkbox checked
                }
                else
                {
                    printf("+<\"uncheck\" Sheet %s>\n", categoryPath);  // checkbox unchecked
                }
            }
            else
            {
                printf("+<Sheet %s>\n", categoryPath);  // no checkbox
            }
        }
        else if (!strcmp(node->name, ELEM_FEATURE))
        {
            /* Get Feature obj by name */
            FeatureField *featureField;
            int ignore = 0;
            Feature *feature = treeRootGetFeatureByName(treeRoot, xmlNodeGetProp(node, ATTRI_NAME));
            isFeatureNode = 1;

            /* Check feature option */
            if (feature->featureOption && !appHandleIsFeatureOptionEnabled(treeRoot->paramTreeView->audioType->appHandle, feature->featureOption))
            {
                //printf ("Feature %s unsupport (%s is disabled)\n", feature->name, feature->featureOption);
                ignore = 1;
            }

            /* Check category path */
            if (!ignore && !featureIsCategoryPathSupport(feature, categoryPath))
            {
                //printf ("Feature %s unsupport (%s is not valid category path)\n", feature->name, categoryPath);
                ignore = 1;
            }

            if (ignore == 0)
            {
                ParamUnit *paramUnit = audioTypeGetParamUnit(treeRoot->paramTreeView->audioType, categoryPath);

                if (feature->switchFieldInfo)
                {
                    int fieldVal = 0;
                    int onVal = 0;

                    ParamUnit *switchParamUnit = audioTypeGetParamUnit(feature->switchFieldInfo->paramInfo->audioType, categoryPath);
                    paramUnitGetFieldVal(switchParamUnit, feature->switchFieldInfo->paramInfo->name, feature->switchFieldInfo->name, &fieldVal);

                    if ((fieldInfoGetCheckListValue(feature->switchFieldInfo, "on", &onVal) == APP_NO_ERROR)
                        && onVal == fieldVal)
                    {
                        printf("+<\"check\" Feature name = %s>\n", feature->name);  // checkbox checked
                    }
                    else
                    {
                        printf("+<\"uncheck\" Feature name = %s>\n", feature->name);    // checkbox unchecked
                    }
                }
                else
                {
                    printf("+<Feature name = %s>\n", feature->name);    // no checkbox
                }

                for (featureField = feature->featureFieldHash; featureField; featureField = featureField->hh.next)
                {
                    unsigned int fieldVal = 0;
                    paramUnitGetFieldVal(paramUnit,
                                         featureField->fieldInfo->paramInfo->name,
                                         featureField->fieldInfo->name,
                                         &fieldVal);

                    printf("      <Field name = %s, val = %d, check_list=%s>\n",
                           featureField->fieldInfo->name,
                           fieldVal,
                           featureField->fieldInfo->checkListStr);
                }
            }
        }
        else
        {
            printf("+<%s>\n", node->name);
        }
    }

    if (level && node->next)
    {
        processTreeRootNode(node->next, level, treeRoot, categoryPath);
    }

    if (!isFeatureNode && node->children)
    {
        processTreeRootNode(node->children, level + 1, treeRoot, categoryPath);
    }
}

/* Notice: it's just example, the num of categoryType may be 4, 5... you have to get the category path more flexible */
char *queryCategoryPathByWording(AppHandle *appHandle, const char *audioTypeName, const char *categoryType1Wording, const char *category1Wording, const char *categoryType2Wording, const char *categoryGroup2Wording, const char *category2Wording, const char *categoryType3Wording, const char *categoryGroup3Wording, const char *category3Wording)
{
    char *result;
    UT_string *searchPath = NULL;
    CategoryType *categoryType = NULL;
    CategoryGroup *categoryGroup = NULL;
    Category *category = NULL;
    AudioType *audioType = appHandleGetAudioTypeByName(appHandle, audioTypeName);

    /* If user select a category path, just like "NarrowBand / Normal of Handset / Level0" */
    utstring_new(searchPath);

    /* Query first category type name & category name */
    if (audioType)
    {
        categoryType = audioTypeGetCategoryTypeByName(audioType, categoryType1Wording);
    }
    if (categoryType)
    {
        category = categoryTypeGetCategoryByWording(categoryType, category1Wording);
        utstring_printf(searchPath, "%s,%s,", categoryType->name, category->name);
    }

    /* Query 2nd category type name & category name (include category group)*/
    categoryGroup = NULL;
    categoryGroup = NULL;

    if (audioType)
    {
        categoryType = audioTypeGetCategoryTypeByName(audioType, categoryType2Wording);
    }
    if (audioType)
    {
        categoryGroup = categoryTypeGetCategoryGroupByWording(categoryType, categoryGroup2Wording);
    }
    if (categoryGroup)
    {
        category = categoryGroupGetCategoryByWording(categoryGroup, category2Wording);
        if (category)
        {
            utstring_printf(searchPath, "%s,%s,", categoryType->name, category->name);
        }
        else
        {
            printf("Error: Cannot find \"%s\" category from \"%s\" category group.\n", category2Wording, categoryGroup->name);
            utstring_free(searchPath);
            return NULL;
        }
    }

    /* Query 3nd category type name & category name */
    categoryGroup = NULL;
    categoryGroup = NULL;

    if (audioType)
    {
        categoryType = audioTypeGetCategoryTypeByWording(audioType, categoryType3Wording);
    }
    if (categoryType)
    {
        categoryGroup = categoryTypeGetCategoryGroupByWording(categoryType, categoryGroup3Wording);
    }
    if (categoryGroup)
    {
        category = categoryGroupGetCategoryByWording(categoryGroup, category3Wording);
        utstring_printf(searchPath, "%s,%s", categoryType->name, category->name);
    }

    if (searchPath)
    {
        result = strdup(utstring_body(searchPath));
        //printf("==> The param unit search path = %s\n", result);
    }
    else
    {
        printf("Error: cannot get the category path\n");
    }

    utstring_free(searchPath);

    return result;
}

void queryFieldValue(AppHandle *appHandle, const char *targetAudioTypeName, const char *categoryPath, const char *paramName, const char *fieldName)
{
    unsigned int fieldValue;
    ParamUnit *paramUnit;

    AudioType *audioType = appHandleGetAudioTypeByName(appHandle, targetAudioTypeName);

    /* Query the ParamUnit */
    paramUnit = audioTypeGetParamUnit(audioType, categoryPath);

    /* Query the field value */
    if (paramUnitGetFieldVal(paramUnit, paramName, fieldName, &fieldValue) == APP_ERROR)
    {
        printf("Error: Cannot query field value successfully!!\n");
    }
    else
    {
        printf("Field value = 0x%x (%s/%s)\n", fieldValue, paramName, fieldName);
    }
}

void simpleFieldQuery(AppHandle *appHandle)
{
    unsigned int fieldValue;
    ParamUnit *paramUnit;
    Param *param;
    FieldInfo *fieldInfo;

    const char *targetAudioTypeName = "Speech";
    const char *targetParamName = "speech_mode_para";
    const char *targetFieldName = "DL Digital Gain";

    /* Example of category combination */
    const char *categoryType1Wording = "Bandwidth";
    const char *category1Wording = "Narrow Band";

    const char *categoryType2Wording = "Profile";
    const char *categoryGroup2Wording = "Handset";
    const char *category2Wording = "Handset";

    const char *categoryType3Wording = "Volume";
    const char *categoryGroup3Wording = "Index";
    const char *category3Wording = "Level0";

    /* Query category path */
    char *categoryPath = queryCategoryPathByWording(appHandle, targetAudioTypeName, categoryType1Wording, category1Wording, categoryType2Wording, categoryGroup2Wording, category2Wording, categoryType3Wording, categoryGroup3Wording, category3Wording);

    /* Query AudioType */
    AudioType *audioType = appHandleGetAudioTypeByName(appHandle, targetAudioTypeName);
    if (!audioType)
    {
        free(categoryPath);
        return;
    }

    /* Read lock */
    audioTypeReadLock(audioType, __FUNCTION__);

    /* Query the ParamUnit */
    paramUnit = audioTypeGetParamUnit(audioType, categoryPath);
    if (!paramUnit)
    {
        free(categoryPath);
        audioTypeUnlock(audioType);
        return;
    }

    printf("\n\n==== Simple Test Query ====\n");
    printf("AudioType/Param/Field = %s / %s / %s\n", targetAudioTypeName, targetParamName, targetFieldName);
    printf("Category path = %s\n", categoryPath);

    /* Query the param value */
    param = paramUnitGetParamByName(paramUnit, "speech_mode_para");
    if (!param)
    {
        printf("Error: Cannot query param value!\n");
        free(categoryPath);
        audioTypeUnlock(audioType);
        return;
    }
    showParamValue(param);

    /* Query the field value */
    if (paramUnitGetFieldVal(paramUnit, targetParamName, targetFieldName, &fieldValue) == APP_ERROR)
    {
        printf("Error: Cannot query field value!!\n");
        free(categoryPath);
        audioTypeUnlock(audioType);
        return;
    }

    /* Query the field's check list */
    fieldInfo = paramInfoGetFieldInfoByName(param->paramInfo, targetFieldName);
    if (fieldInfo)
    {
        printf("==> Field val = %x (check_list = %s)\n", fieldValue, fieldInfo->checkListStr);
    }
    else
    {
        printf("Error: Cannot find the fieldInfo!\n");
    }

    free(categoryPath);

    /* Unlock */
    audioTypeUnlock(audioType);
}

void simpleParamQuery(AppHandle *appHandle)
{
    /* Query category path */
    char *audioTypeName = "Speech";
    char *categoryPath = "Band,NB,Profile,4_pole_Headset,VolIndex,3";
    char *paramName = "speech_mode_para";
    ParamUnit *paramUnit;
    Param *param;

    /* Query AudioType */
    AudioType *audioType = appHandleGetAudioTypeByName(appHandle, audioTypeName);
    if (!audioType)
    {
        return;
    }

    /* Read Lock */
    audioTypeReadLock(audioType, __FUNCTION__);

    /* Query the ParamUnit */
    paramUnit = audioTypeGetParamUnit(audioType, categoryPath);
    if (!paramUnit)
    {
        return;
    }

    printf("\n\n==== Simple Test Query ====\n");
    printf("AudioType/Param/Field = %s / %s\n", audioTypeName, paramName);
    printf("Category path = %s\n", categoryPath);

    /* Query the param value */
    param = paramUnitGetParamByName(paramUnit, paramName);
    if (!param)
    {
        printf("Error: Cannot query param value!\n");
        return;
    }
    showParamValue(param);

    /* Read unlock */
    audioTypeUnlock(audioType);
}

void simpleParamUpdate(AppHandle *appHandle)
{
    unsigned short shortArray[] = {0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666, 0x7777, 0x8888, 0x9999, 0x0000};
    int arraySize = 10;

    /* You should cache follow object in somewhere without query again */
    AudioType *audioType = appHandleGetAudioTypeByName(appHandle, "Speech");
    ParamInfo *paramInfo = audioTypeGetParamInfoByName(audioType, "sph_in_fir");

    /* The sph_in_fir param is short array type */
    if (audioTypeSetParamData(audioType, "Band,NB,Profile,HAC,VolIndex,3,Network,GSM", paramInfo, (void *)shortArray, arraySize) == APP_ERROR)
    {
        printf("Cannot update the param data!!\n");
    }
}

void simpleFieldUpdate(AppHandle *appHandle)
{
    unsigned int fieldVal = 0xff;

    /* You should cache follow object in somewhere without query again */
    AudioType *audioType = appHandleGetAudioTypeByName(appHandle, "Speech");
    ParamInfo *paramInfo = audioTypeGetParamInfoByName(audioType, "speech_mode_para");
    FieldInfo *fieldInfo = paramInfoGetFieldInfoByName(paramInfo, "DL Digital Gain");

    /* Update the fieldInfo for specific categoryPath */
    if (audioTypeSetFieldData(audioType, "Band,NB,Profile,HAC,VolIndex,3,Network,GSM", fieldInfo, fieldVal) == APP_ERROR)
    {
        printf("Cannot update the field data!!\n");
    }
}

void applyParamUnitToCategory(AppHandle *appHandle)
{
    const char *srcCategoryPath = "Band,NB,Profile,HAC,VolIndex,3,Network,GSM";
    const char *dstCategoryPath = "Band,NB,Profile,HAC,VolIndex,4,Network,GSM";

    /* Query AudioType */
    AudioType *audioType = appHandleGetAudioTypeByName(appHandle, "Speech");

    /* Apply the ParamUnit */
    audioTypeParamUnitCopy(audioType, srcCategoryPath, dstCategoryPath);
}

void saveModifiedAudioParamXml(AppHandle *appHandle, const char *folder)
{
    size_t i;
    for (i = 0; i < appHandleGetNumOfAudioType(appHandle); i++)
    {
        AudioType *audioType = appHandleGetAudioTypeByIndex(appHandle, i);

        /* Read lock */
        audioTypeReadLock(audioType, __FUNCTION__);

        if (audioType->dirty && audioTypeSaveAudioParamXml(audioType, folder, 1) == APP_ERROR)
        {
            printf("Error: cannot save audio param XML to %s dir\n", folder);
        }

        /* Unlock */
        audioTypeUnlock(audioType);
    }
}

void xmlChangedCallback(AppHandle *appHandle, const char *audioTypeName)
{
    printf("XML file changed. (cus folder = %s, audioType = %s)\n", appHandle->xmlCusDir, audioTypeName);

    // reload XML file
    if (appHandleReloadAudioType(appHandle, audioTypeName) == APP_ERROR)
    {
        printf("Reload xml fail! (audioType = %s)\n", audioTypeName);
    }
    else
    {
#if 0
        AudioType *audioType;
        ParamUnit *paramUnit;
        Param *param;

        printf("Reload XML done. (audioType = %s)\n", audioTypeName);

        /* Query AudioType */
        audioType = appHandleGetAudioTypeByName(appHandle, audioTypeName);
        if (!audioType)
        {
            printf("Cannot find %s audio type\n", audioTypeName);
            return;
        }

        /* Query the ParamUnit */
        paramUnit = audioTypeGetParamUnit(audioType, "Band,NB,Profile,Normal,VolIndex,0,Network,GSM");
        if (!paramUnit)
        {
            printf("Cannot find paramUnit\n");
            return;
        }

        /* Query the param value */
        param = paramUnitGetParamByName(paramUnit, "speech_mode_para");
        if (!param)
        {
            printf("Error: Cannot query param value!\n");
            return;
        }
        showParamValue(param);
#else
        printf("Reload XML done. (audioType = %s)\n", audioTypeName);
        utilUsleep(2000000000);
        printf("Sleep 2 sec done...\n");
#endif
    }
}

int showDynamicTest(AppHandle *appHandle)
{
    AudioType *audioType = NULL;
    char *categoryPath = NULL;
    char *paramName = NULL;
    ParamUnit *paramUnit = NULL;
    ParamInfo *paramInfo = NULL;
    FieldInfo *fieldInfo = NULL;
    Param *param = NULL;
    void *paramData = NULL;
    size_t arraySize = 0;
    char tmp[BUF_SIZE];
    char *input;
    unsigned int fieldValue = 0;

    printf("\n\n====== Dynamic Test =====\n");
    printf("[0] Back to main menu\n");
    printf("[1] Get param value\n");
    printf("[2] Set param value\n");
    printf("[3] Get field value\n");
    printf("[4] Set field value\n");
    printf("[5] ParamUnit copy\n");
    printf("[6] Save xml\n");
    printf("[7] Show param tree view\n");
    printf("[8] Set switchField on/off \n");
    printf("[9] Compress files\n");
    printf("[10] UnCompress file\n");
    printf("[11] AudioManager.getParameters\n");
    printf("[12] AudioManager.setParameters\n");
    printf("==========================\n");
    printf("Please enter the selection: ");
    input = utilGetStdin(tmp, BUF_SIZE);

    if (!strcmp(input, "0"))
    {
        return 0;
    }
    else if (!strcmp(input, "1"))
    {
        printf("Enter audio type name (eg. Speech):");
        input = utilGetStdin(tmp, BUF_SIZE);

        audioType = appHandleGetAudioTypeByName(appHandle, input);
        if (audioType)
        {
            printf("Enter category path (eg. Band,NB,Profile,4_pole_Headset,VolIndex,3,Network,GSM):");
            input = utilGetStdin(tmp, BUF_SIZE);

            /* Read lock */
            audioTypeReadLock(audioType, __FUNCTION__);

            paramUnit = audioTypeGetParamUnit(audioType, input);
            if (paramUnit)
            {
                printf("Enter param name (eg. speech_mode_para):");
                input = utilGetStdin(tmp, BUF_SIZE);

                param = paramUnitGetParamByName(paramUnit, input);
                if (param)
                {
                    showParamValue(param);
                }
                else
                {
                    printf("Error: Cannot find the param!\n");
                }
            }
            else
            {
                printf("Error: Cannot find the param unit!\n");
            }

            /* Unlock */
            audioTypeUnlock(audioType);
        }
        else
        {
            printf("Error: no such audio type\n");
        }
    }
    else if (!strcmp(input, "2"))
    {
        printf("Enter audio type name (eg. Speech):");
        input = utilGetStdin(tmp, BUF_SIZE);

        audioType = appHandleGetAudioTypeByName(appHandle, input);
        if (audioType)
        {
            printf("Enter param name (eg. speech_mode_para):");
            input = utilGetStdin(tmp, BUF_SIZE);

            paramInfo = audioTypeGetParamInfoByName(audioType, input);
            if (paramInfo)
            {
                printf("Enter category path (eg. Band,NB,Profile,4_pole_Headset,VolIndex,3,Network,GSM):");
                input = utilGetStdin(tmp, BUF_SIZE);

                categoryPath = strdup(input);

                printf("Enter param value (type:%s):", paramDataTypeToStr(paramInfo->dataType));
                input = utilGetStdin(tmp, BUF_SIZE);

                if (utilConvDataStringToNative(paramInfo->dataType, input, &paramData, &arraySize) == APP_NO_ERROR)
                {
                    /* The sph_in_fir param is short array type */
                    if (audioTypeSetParamData(audioType, categoryPath, paramInfo, (void *)paramData, arraySize) == APP_ERROR)
                    {
                        printf("Cannot update the param data!!\n");
                    }
                }

                free(categoryPath);
            }
            else
            {
                printf("Error: cannot find the param!\n");
            }
        }
        else
        {
            printf("Error: no such audio type\n");
        }
    }
    else if (!strcmp(input, "3"))
    {
        printf("Enter audio type name (eg. Speech):");
        input = utilGetStdin(tmp, BUF_SIZE);

        audioType = appHandleGetAudioTypeByName(appHandle, input);
        if (audioType)
        {
            printf("Enter category path (eg. Band,NB,Profile,4_pole_Headset,VolIndex,3,Network,GSM):");
            input = utilGetStdin(tmp, BUF_SIZE);

            /* Read lock */
            audioTypeReadLock(audioType, __FUNCTION__);

            paramUnit = audioTypeGetParamUnit(audioType, input);
            if (paramUnit)
            {
                printf("Enter param name (eg. speech_mode_para):");
                input = utilGetStdin(tmp, BUF_SIZE);

                paramInfo = audioTypeGetParamInfoByName(audioType, input);
                if (paramInfo)
                {
                    printf("Enter field name (eg. DL Digital Gain):");
                    input = utilGetStdin(tmp, BUF_SIZE);

                    if (paramUnitGetFieldVal(paramUnit, paramInfo->name, input, &fieldValue) == APP_ERROR)
                    {
                        printf("Error: Cannot query field value!\n");
                    }
                    else
                    {
                        printf("Field value = 0x%x\n", fieldValue);
                    }
                }
                else
                {
                    printf("Error: Cannot find the param!\n");
                }
            }
            else
            {
                printf("Error: Cannot find the param unit!\n");
            }

            /* Unlock */
            audioTypeUnlock(audioType);
        }
        else
        {
            printf("Error: no such audio type\n");
        }
    }
    else if (!strcmp(input, "4"))
    {
        printf("Enter audio type name (eg. Speech):");
        input = utilGetStdin(tmp, BUF_SIZE);

        audioType = appHandleGetAudioTypeByName(appHandle, input);
        if (audioType)
        {
            printf("Enter category path (eg. Band,NB,Profile,4_pole_Headset,VolIndex,3,Network,GSM):");
            categoryPath = strdup(utilGetStdin(tmp, BUF_SIZE));

            printf("Enter param name (eg. speech_mode_para):");
            input = utilGetStdin(tmp, BUF_SIZE);

            paramInfo = audioTypeGetParamInfoByName(audioType, input);
            if (paramInfo)
            {
                printf("Enter field name (eg. DL Digital Gain):");
                input = utilGetStdin(tmp, BUF_SIZE);

                fieldInfo = paramInfoGetFieldInfoByName(paramInfo, input);
                if (fieldInfo)
                {
                    printf("Enter field value:");
                    input = utilGetStdin(tmp, BUF_SIZE);

                    if (audioTypeSetFieldData(audioType, categoryPath, fieldInfo, strtoul(input, NULL, 0)) == APP_NO_ERROR)
                    {
                        printf("Set field value = 0x%zx\n", strtoul(input, NULL, 0));
                    }
                    else
                    {
                        printf("Error: Cannot set field value!\n");
                    }
                }
                else
                {
                    printf("Error: Cannot find the field!\n");
                }
            }
            else
            {
                printf("Error: Cannot find the param!\n");
            }

            free(categoryPath);
        }
        else
        {
            printf("Error: no such audio type\n");
        }
    }
    else if (!strcmp(input, "5"))
    {
        printf("Enter audio type name (eg. Speech):");
        input = utilGetStdin(tmp, BUF_SIZE);

        audioType = appHandleGetAudioTypeByName(appHandle, input);
        if (audioType)
        {
            char *src = NULL;
            printf("Enter src category path (eg. Band,NB,Profile,HAC,VolIndex,3,Network,GSM):");
            input = utilGetStdin(tmp, BUF_SIZE);
            src = strdup(input);

            printf("Enter dst category path (eg. Band,NB,Profile,HAC,VolIndex,4,Network,GSM):");
            input = utilGetStdin(tmp, BUF_SIZE);

            if (audioTypeParamUnitCopy(audioType, src, input))
            {
                printf("ParamUnit copied\n");
            }
            else
            {
                printf("Error: Cannot copy paramUnit!\n");
            }

            free(src);
        }
        else
        {
            printf("Error: no such audio type\n");
        }
    }
    else if (!strcmp(input, "6"))
    {
        printf("Enter audio type name (eg. Speech):");
        input = utilGetStdin(tmp, BUF_SIZE);

        audioType = appHandleGetAudioTypeByName(appHandle, input);
        if (audioType)
        {

#ifdef WIN32
            printf("Enter folder to save XML (eg. .\\cus):");
#else
            printf("Enter folder to save XML (eg. /sdcard/.audio_param/):");
#endif
            input = utilGetStdin(tmp, BUF_SIZE);

            /* Read lock */
            audioTypeReadLock(audioType, __FUNCTION__);

            /* Save changed AudioType to XML */
            audioTypeSaveAudioParamXml(audioType, input, 1);

            /* Unlock */
            audioTypeUnlock(audioType);
        }
        else
        {
            printf("Error: no such audio type\n");
        }
    }
    else if (!strcmp(input, "7"))
    {
        char *treeRootName;
        AudioType *audioType;

        printf("Enter audio type name (eg. Speech):");
        input = utilGetStdin(tmp, BUF_SIZE);
        audioType = appHandleGetAudioTypeByName(appHandle, input);
        if (audioType)
        {
            TreeRoot *treeRoot;
            printf("Enter tree root name (eg. NREC):");
            input = utilGetStdin(tmp, BUF_SIZE);
            treeRootName = strdup(input);
            treeRoot = audioTypeGetTreeRoot(audioType, treeRootName);
            if (treeRoot)
            {
                printf("Enter category path (eg. Band,NB,Profile,HAC,VolIndex,3,Network,GSM):");
                input = utilGetStdin(tmp, BUF_SIZE);

                /* Show tree root */
                processTreeRootNode(treeRoot->treeRootNode, 0, treeRoot, input);
            }
            else
            {
                printf("Error: Cannot find the %s tree root!\n", treeRootName);
            }
            free(treeRootName);
        }
        else
        {
            printf("Error: Cannot find %s audio type!\n", input);
        }
    }
    else if (!strcmp(input, "8"))
    {
        printf("Enter audio type name (eg. Speech):");
        input = utilGetStdin(tmp, BUF_SIZE);

        audioType = appHandleGetAudioTypeByName(appHandle, input);
        if (audioType)
        {
            printf("Enter category path (eg. Band,NB,Profile,4_pole_Headset,VolIndex,3,Network,GSM):");
            categoryPath = strdup(utilGetStdin(tmp, BUF_SIZE));

            printf("Enter param name (eg. speech_mode_para):");
            input = utilGetStdin(tmp, BUF_SIZE);

            paramInfo = audioTypeGetParamInfoByName(audioType, input);
            if (paramInfo)
            {
                FieldInfo *switchFieldInfo;
                printf("Enter field name (eg. switch):");
                input = utilGetStdin(tmp, BUF_SIZE);

                switchFieldInfo = paramInfoGetFieldInfoByName(paramInfo, input);

                /* For parameter tree, you can get the fieldInfo by treeRoot->switchFieldInfo & feature-> switchFieldInfo*/
                if (switchFieldInfo)
                {
                    printf("Enter switch on/off (1/0):");
                    input = utilGetStdin(tmp, BUF_SIZE);

                    if (!strcmp(input, "1"))
                    {
                        /* Get the check list on's value */
                        unsigned int onValue;
                        if (fieldInfoGetCheckListValue(switchFieldInfo, "on", &onValue) == APP_ERROR)
                        {
                            printf("Error: Cannot get the check list's on value! (XML should define the on's value)\n");
                        }
                        else
                        {
                            /* Set the field with on's value */
                            if (audioTypeSetFieldData(switchFieldInfo->paramInfo->audioType, categoryPath, switchFieldInfo, onValue) == APP_ERROR)
                            {
                                printf("Cannot set the filed data successfully!\n");
                            }
                            else
                            {
                                printf("Set the field data successfully!\n");
                            }
                        }
                    }
                    else
                    {
                        /* Get the check list off's value */
                        unsigned int offValue;
                        if (fieldInfoGetCheckListValue(switchFieldInfo, "off", &offValue) == APP_ERROR)
                        {
                            printf("Error: Cannot get the check list's off value! (XML should define the off's value)\n");
                        }
                        else
                        {
                            /* Set the field with off's value */
                            if (audioTypeSetFieldData(switchFieldInfo->paramInfo->audioType, categoryPath, switchFieldInfo, offValue) == APP_ERROR)
                            {
                                printf("Cannot set the filed data successfully!\n");
                            }
                            else
                            {
                                printf("Set the field data successfully!\n");
                            }
                        }
                    }
                }
                else
                {
                    printf("Error: Cannot find the field!\n");
                }
            }
            else
            {
                printf("Error: No fieldInfo found!\n");
            }

            free(categoryPath);
        }
        else
        {
            printf("Error: no such audio type\n");
        }
    }
    else if (!strcmp(input, "9"))
    {
        char* srcDir;
        char* dstFile;
        printf("Enter compress folder full path: ");
        srcDir = strdup(utilGetStdin(tmp, BUF_SIZE));

        printf("Enter target file full path: ");
        dstFile = strdup(utilGetStdin(tmp, BUF_SIZE));

        if (appHandleCompressFiles(srcDir, dstFile) == APP_ERROR)
        {
            printf("File compress fail\n");
        } else {
            printf("File compress done\n");
        }
        free(srcDir);
        free(dstFile);
    }
    else if (!strcmp(input, "10"))
    {
        char* srcFile;
        char* dstDir;
        printf("Enter src file full path: ");
        srcFile = strdup(utilGetStdin(tmp, BUF_SIZE));

        printf("Enter target dir full path: ");
        dstDir = strdup(utilGetStdin(tmp, BUF_SIZE));

        if (appHandleUncompressFile(srcFile, dstDir) == APP_ERROR)
        {
            printf("File uncompress fail\n");
        } else {
            printf("File uncompress done\n");
        }
        free(srcFile);
        free(dstDir);
    }
    else if (!strcmp(input, "11"))
    {
#ifndef WIN32
        char tmp[BUF_SIZE];
        char* input, *res;
        printf("Please enter the parameter name: (ex: GET_CUST_XML_ENABLE)\n");
        input = utilGetStdin(tmp, BUF_SIZE);
        res = audioSystemGetParameters(input);
        printf("return: %s\n", res);
        free(res);
#else
        printf("Not support this test on win32\n");
#endif
    }
    else if (!strcmp(input, "12"))
    {
#ifndef WIN32
        char tmp[BUF_SIZE];
        char* input, *res;
        printf("Please enter the parameter name: (ex: SET_CUST_XML_ENABLE=1)\n");
        input = utilGetStdin(tmp, BUF_SIZE);
        audioSystemSetParameters(input);
#else
        printf("Not support this test on win32\n");
#endif
    }
    return 1;
}

void testAppLibAPIs()
{
    AppHandle *appHandle = appHandleGetInstance();
    int oldDebugLevel = appGetDebugLevel();
    AudioType *audioType = appHandleGetAudioTypeByName(appHandle, "Speech");
    ParamInfo *paramInfo = audioTypeGetParamInfoByName(audioType, "speech_mode_para");
    FieldInfo *fieldInfo = paramInfoGetFieldInfoByName(paramInfo, "AEC");

    /* Change level to debug */
    appSetDebugLevel(DEBUG_LEVEL);

    /* Set AEC field */
    audioTypeSetFieldData(audioType, "Band,NB,Profile,Normal,VolIndex,3,Network,GSM", fieldInfo, 0xbd);

    /* Copy ParamUnit src = dst */
    audioTypeParamUnitCopy(audioType, "Band,NB,Profile,Normal,VolIndex,3,Network,GSM", "Band,NB,Profile,Normal,VolIndex,3,Network,GSM");

    /* Copy ParamUnit src != dst */
    audioTypeParamUnitCopy(audioType, "Band,NB,Profile,Normal,VolIndex,3,Network,GSM", "Band,NB,Profile,HAC,VolIndex,3,Network,GSM");

    /* Query param */
    simpleParamQuery(appHandle);

    /* Retrieve specific audio type's param & field information */
    simpleFieldQuery(appHandle);

    /* Query non-exist ParamUnit, it shouldn't crash */
    audioTypeGetParamUnit(audioType, "aaa,bbb");

    /* Update param value */
    simpleParamUpdate(appHandle);

    /* Update field value */
    simpleFieldUpdate(appHandle);

    /* Apply param to other category */
    applyParamUnitToCategory(appHandle);

#ifndef WIN32
    /* Save changed AudioType to XML */
    saveModifiedAudioParamXml(appHandle, XML_CUS_FOLDER_ON_DEVICE);
#else
    /* Save changed AudioType to XML */
    saveModifiedAudioParamXml(appHandle, XML_CUS_FOLDER_ON_TUNING_TOOL);
#endif
    appSetDebugLevel(oldDebugLevel);
}

void testUtilNativeAPIs()
{
    APP_STATUS status;
    unsigned int fieldResult = 0;
    char *strResult;

    /* Get category string */
    strResult = utilNativeGetCategory("Speech", "Band");
    printf("Category result = %s\n", strResult);
#ifndef WIN32
    free(strResult);
#else
    printf("Cannot free the memory allocated by APP on WIN32, just for testing\n");
#endif

    /* Set param */
    status = utilNativeSetParam("SpeechGeneral", "CategoryLayer,Common", "speech_common_para", "0x1,0xDABD,0x7918,0x2A00,0x8001,0x0,0x0,0x0,0x0,0x0,0x0,0x0");
    if (status == APP_ERROR)
    {
        printf("utilNativeSetParam fail!\n");
        exit(1);
    }

    /* Get param */
    strResult = utilNativeGetParam("SpeechGeneral", "CategoryLayer,Common", "speech_common_para");
    printf("Param = %s\n", strResult);
#ifndef WIN32
    free(strResult);
#else
    printf("Cannot free the memory allocated by APP on WIN32, just for testing\n");
#endif

    /* Set field */
    status = utilNativeSetField("Speech", "Band,NB,Profile,Normal", "speech_mode_para", "AEC", "252");
    if (status == APP_ERROR)
    {
        printf("utilNativeSetField fail!\n");
        exit(1);
    }

    /* Get field */
    fieldResult = utilNativeGetField("Speech", "Band,NB,Profile,Normal", "speech_mode_para", "AEC");
    printf("Field = 0x%x\n", fieldResult);

    /* Get check list */
    strResult = utilNativeGetChecklist("Speech", "speech_mode_para", "AEC");
    printf("Check list = %s\n", strResult);

    /* Save parameter */
    status = utilNativeSaveXml("Speech");
    if (status == APP_ERROR)
    {
        printf("utilNativeSaveXml fail!\n");
        exit(1);
    }
}

int showXmlInfo(AppHandle *appHandle)
{
    AudioType *audioType = NULL;
    char *categoryPath = NULL;
    char *paramName = NULL;
    ParamUnit *paramUnit = NULL;
    ParamInfo *paramInfo = NULL;
    FieldInfo *fieldInfo = NULL;
    Param *param = NULL;
    void *paramData = NULL;
    size_t arraySize = 0;
    char tmp[BUF_SIZE];
    char *input;
    unsigned int fieldValue = 0;

    printf("\n\n====== Show XML Information =====\n");
    printf("[0] Back to main menu\n");
    printf("[1] Show all audio type info\n");
    printf("[2] Show all category info\n");
    printf("[3] Show all param/field info\n");
    printf("[4] Show all param tree view info\n");
    printf("[5] Show feature options\n");
    printf("==========================\n");
    printf("Please enter the selection: ");
    input = utilGetStdin(tmp, BUF_SIZE);

    if (!strcmp(input, "0"))
    {
        return 0;
    }
    else if (!strcmp(input, "1"))
    {
        showAllAudioType(appHandle);
    }
    else if (!strcmp(input, "2"))
    {
        showAllCategoryInformation(appHandle);
    }
    else if (!strcmp(input, "3"))
    {
        showAllParamFieldInformation(appHandle);
    }
    else if (!strcmp(input, "4"))
    {
        showAllParamTreeInformation(appHandle);
    }
    else if (!strcmp(input, "5"))
    {
        showFeatureOptions(appHandle);
    }
    return 1;
}

int showMainMenu(AppHandle *appHandle)
{
    char tmp[BUF_SIZE];
    char *input;
    printf("\n\n======= Main Menu =======\n");
    printf("[0] Exit\n");
    printf("[1] Unit test\n");
    printf("[2] Show XML information (AudioType/Category/ParamInfo/FieldInfo)\n");
    printf("[3] Test formal Speech audio type xml. (Fixed test pattern)\n");
    printf("[4] File changed callback test (only support on android version)\n");
    printf("[5] Dynamic operation test\n");
    printf("[6] Set debug level\n");
    printf("[7] Show log to console\n");
    printf("[8] Show app lib build timestamp\n");
    printf("==========================\n");
    printf("Please enter the selection: ");
    input = utilGetStdin(tmp, BUF_SIZE);

    if (!strcmp(input, "0"))
    {
        return 0;
    }
    else if (!strcmp(input, "1"))
    {
        /* APP Parser internal unit test */
        if (unitTest(appHandle) == APP_ERROR)
        {
            printf("Unit test failure!\n");
        }
        else
        {
            printf("Unit test pass!\n");
        }
    }
    else if (!strcmp(input, "2"))
    {
        while (1)
        {
            if (showXmlInfo(appHandle) == 0)
            {
                break;
            }
        }
    }
    else if (!strcmp(input, "3"))
    {
        testAppLibAPIs();

        printf("Press enter to continuous next test:");
        input = utilGetStdin(tmp, BUF_SIZE);

        /* Test 2 */
        testUtilNativeAPIs();
    }
    else if (!strcmp(input, "4"))
    {
#ifndef WIN32
        /* XML changed callback process example for audio driver */
        appHandleRegXmlChangedCb(appHandle, xmlChangedCallback);
        printf("Please push AudioParam xml to %s folder to test the xml changed callback\n", XML_CUS_FOLDER_ON_DEVICE);
        printf("You can press any key to continue!\n");;
        getchar();  // waiting inotify thead loop
#else
        printf("Not support windows version\n");
#endif
    }
    else if (!strcmp(input, "5"))
    {
        while (1)
        {
            if (showDynamicTest(appHandle) == 0)
            {
                break;
            }
        }
    }
    else if (!strcmp(input, "6"))
    {
        int level;
        printf("Enter debug level (eg. 0:DEBUG, 1:INFO, 2:WARN, 3:ERR):");
        input = utilGetStdin(tmp, BUF_SIZE);

        level = (int)strtoul(input, NULL, 0);
        if (level < 0 || level > ERR_LEVEL)
        {
            printf("Invalid level value. (%d)\n", level);
        }
        else
        {
            appSetDebugLevel(level);
            printf("Set debug level = %d\n", level);
        }
    }
    else if (!strcmp(input, "7"))
    {
        appHandleRedirectIOToConsole();
    }
    else if (!strcmp(input, "8"))
    {
        printf("APP lib building time stamp: %s\n", appHandleGetBuildTimeStamp());
    }
    return 1;
}

int main()
{

    AppHandle *appHandle = NULL;

#ifdef WIN32
    AppHandle Handle;
    /* For Tuning Tool debug usage, used to show the APP lib message to the console */
    appHandleRedirectIOToConsole();

    /* Init app handle */
    appHandleInit(&Handle);
    appHandle = &Handle;

    /* Parse the xml in default and cus folder,
      if cus folder has the same name of XML file,
      parser will load the cus folder xml instead of default xml folder */
    appHandleParseXml(appHandle, XML_FOLDER_ON_TUNING_TOOL, XML_CUS_FOLDER_ON_TUNING_TOOL);
#else
    /* Get AppHandle global instance, this API will parse xml automatically */
    appHandle = appHandleGetInstance();
#endif

    /* Set the debug level, default is INFO_LEVEL */
    appSetDebugLevel(INFO_LEVEL);

    while (1)
    {
        if (showMainMenu(appHandle) == 0)
        {
            break;
        }
    }

    /* Release appHandle resources */
    appHandleUninit(appHandle);

    return 0;
}

