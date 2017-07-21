package com.mediatek.engineermode.tests;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

//use below command to run test case marked with annotation of "BasicFuncAnnotation"
//adb shell am instrument  -w -e annotation
//  com.mediatek.engineermode.tests.BasicFuncAnnotation
//  com.mediatek.engineermode.tests/.EmFunctionalTestRunner

@Retention(RetentionPolicy.RUNTIME)
@Target({ElementType.METHOD, ElementType.TYPE })
public @interface BasicFuncAnnotation {

}
