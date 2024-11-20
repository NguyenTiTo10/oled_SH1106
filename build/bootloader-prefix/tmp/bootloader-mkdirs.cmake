# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/MTC-TTM/esp/v5.2.3/esp-idf/components/bootloader/subproject"
  "E:/Projects/ESP_IDF/Component/oled_sh1106/build/bootloader"
  "E:/Projects/ESP_IDF/Component/oled_sh1106/build/bootloader-prefix"
  "E:/Projects/ESP_IDF/Component/oled_sh1106/build/bootloader-prefix/tmp"
  "E:/Projects/ESP_IDF/Component/oled_sh1106/build/bootloader-prefix/src/bootloader-stamp"
  "E:/Projects/ESP_IDF/Component/oled_sh1106/build/bootloader-prefix/src"
  "E:/Projects/ESP_IDF/Component/oled_sh1106/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/Projects/ESP_IDF/Component/oled_sh1106/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/Projects/ESP_IDF/Component/oled_sh1106/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
