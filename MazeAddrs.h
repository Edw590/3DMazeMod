// Copyright 2024 Edw590
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#ifndef INC_3DMAZEMOD_MAZEADDRS_H
#define INC_3DMAZEMOD_MAZEADDRS_H



#include <stdint.h>

#define wglMakeCurrent_EXE 0x10012A8
#define glGetIntegerv_EXE 0x100126C

#define gbTurboMode 0x101CFC8

uint32_t CreateSSWindow_EXE = 0x100967D;
uint32_t SetupInitialWindows_EXE = 0x1008CB6;
uint32_t UpdateWindow_EXE = 0x1009FC1;



#endif //INC_3DMAZEMOD_MAZEADDRS_H
