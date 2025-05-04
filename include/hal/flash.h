#ifndef HAL_FLASH_H
#define HAL_FLASH_H

#include "hal/hal_err.h"
#include "stm32wbxx.h"
#include "utils/utils.h"

#include <stdbool.h>

typedef enum {
    FLASH_PAGE_0 = 0x08000000U,   /* Base @ of Page 0, 4 Kbytes */
    FLASH_PAGE_1 = 0x08001000U,   /* Base @ of Page 1, 4 Kbytes */
    FLASH_PAGE_2 = 0x08002000U,   /* Base @ of Page 2, 4 Kbytes */
    FLASH_PAGE_3 = 0x08003000U,   /* Base @ of Page 3, 4 Kbytes */
    FLASH_PAGE_4 = 0x08004000U,   /* Base @ of Page 4, 4 Kbytes */
    FLASH_PAGE_5 = 0x08005000U,   /* Base @ of Page 5, 4 Kbytes */
    FLASH_PAGE_6 = 0x08006000U,   /* Base @ of Page 6, 4 Kbytes */
    FLASH_PAGE_7 = 0x08007000U,   /* Base @ of Page 7, 4 Kbytes */
    FLASH_PAGE_8 = 0x08008000U,   /* Base @ of Page 8, 4 Kbytes */
    FLASH_PAGE_9 = 0x08009000U,   /* Base @ of Page 9, 4 Kbytes */
    FLASH_PAGE_10 = 0x0800A000U,  /* Base @ of Page 10, 4 Kbytes */
    FLASH_PAGE_11 = 0x0800B000U,  /* Base @ of Page 11, 4 Kbytes */
    FLASH_PAGE_12 = 0x0800C000U,  /* Base @ of Page 12, 4 Kbytes */
    FLASH_PAGE_13 = 0x0800D000U,  /* Base @ of Page 13, 4 Kbytes */
    FLASH_PAGE_14 = 0x0800E000U,  /* Base @ of Page 14, 4 Kbytes */
    FLASH_PAGE_15 = 0x0800F000U,  /* Base @ of Page 15, 4 Kbytes */
    FLASH_PAGE_16 = 0x08010000U,  /* Base @ of Page 16, 4 Kbytes */
    FLASH_PAGE_17 = 0x08011000U,  /* Base @ of Page 17, 4 Kbytes */
    FLASH_PAGE_18 = 0x08012000U,  /* Base @ of Page 18, 4 Kbytes */
    FLASH_PAGE_19 = 0x08013000U,  /* Base @ of Page 19, 4 Kbytes */
    FLASH_PAGE_20 = 0x08014000U,  /* Base @ of Page 20, 4 Kbytes */
    FLASH_PAGE_21 = 0x08015000U,  /* Base @ of Page 21, 4 Kbytes */
    FLASH_PAGE_22 = 0x08016000U,  /* Base @ of Page 22, 4 Kbytes */
    FLASH_PAGE_23 = 0x08017000U,  /* Base @ of Page 23, 4 Kbytes */
    FLASH_PAGE_24 = 0x08018000U,  /* Base @ of Page 24, 4 Kbytes */
    FLASH_PAGE_25 = 0x08019000U,  /* Base @ of Page 25, 4 Kbytes */
    FLASH_PAGE_26 = 0x0801A000U,  /* Base @ of Page 26, 4 Kbytes */
    FLASH_PAGE_27 = 0x0801B000U,  /* Base @ of Page 27, 4 Kbytes */
    FLASH_PAGE_28 = 0x0801C000U,  /* Base @ of Page 28, 4 Kbytes */
    FLASH_PAGE_29 = 0x0801D000U,  /* Base @ of Page 29, 4 Kbytes */
    FLASH_PAGE_30 = 0x0801E000U,  /* Base @ of Page 30, 4 Kbytes */
    FLASH_PAGE_31 = 0x0801F000U,  /* Base @ of Page 31, 4 Kbytes */
    FLASH_PAGE_32 = 0x08020000U,  /* Base @ of Page 32, 4 Kbytes */
    FLASH_PAGE_33 = 0x08021000U,  /* Base @ of Page 33, 4 Kbytes */
    FLASH_PAGE_34 = 0x08022000U,  /* Base @ of Page 34, 4 Kbytes */
    FLASH_PAGE_35 = 0x08023000U,  /* Base @ of Page 35, 4 Kbytes */
    FLASH_PAGE_36 = 0x08024000U,  /* Base @ of Page 36, 4 Kbytes */
    FLASH_PAGE_37 = 0x08025000U,  /* Base @ of Page 37, 4 Kbytes */
    FLASH_PAGE_38 = 0x08026000U,  /* Base @ of Page 38, 4 Kbytes */
    FLASH_PAGE_39 = 0x08027000U,  /* Base @ of Page 39, 4 Kbytes */
    FLASH_PAGE_40 = 0x08028000U,  /* Base @ of Page 40, 4 Kbytes */
    FLASH_PAGE_41 = 0x08029000U,  /* Base @ of Page 41, 4 Kbytes */
    FLASH_PAGE_42 = 0x0802A000U,  /* Base @ of Page 42, 4 Kbytes */
    FLASH_PAGE_43 = 0x0802B000U,  /* Base @ of Page 43, 4 Kbytes */
    FLASH_PAGE_44 = 0x0802C000U,  /* Base @ of Page 44, 4 Kbytes */
    FLASH_PAGE_45 = 0x0802D000U,  /* Base @ of Page 45, 4 Kbytes */
    FLASH_PAGE_46 = 0x0802E000U,  /* Base @ of Page 46, 4 Kbytes */
    FLASH_PAGE_47 = 0x0802F000U,  /* Base @ of Page 47, 4 Kbytes */
    FLASH_PAGE_48 = 0x08030000U,  /* Base @ of Page 48, 4 Kbytes */
    FLASH_PAGE_49 = 0x08031000U,  /* Base @ of Page 49, 4 Kbytes */
    FLASH_PAGE_50 = 0x08032000U,  /* Base @ of Page 50, 4 Kbytes */
    FLASH_PAGE_51 = 0x08033000U,  /* Base @ of Page 51, 4 Kbytes */
    FLASH_PAGE_52 = 0x08034000U,  /* Base @ of Page 52, 4 Kbytes */
    FLASH_PAGE_53 = 0x08035000U,  /* Base @ of Page 53, 4 Kbytes */
    FLASH_PAGE_54 = 0x08036000U,  /* Base @ of Page 54, 4 Kbytes */
    FLASH_PAGE_55 = 0x08037000U,  /* Base @ of Page 55, 4 Kbytes */
    FLASH_PAGE_56 = 0x08038000U,  /* Base @ of Page 56, 4 Kbytes */
    FLASH_PAGE_57 = 0x08039000U,  /* Base @ of Page 57, 4 Kbytes */
    FLASH_PAGE_58 = 0x0803A000U,  /* Base @ of Page 58, 4 Kbytes */
    FLASH_PAGE_59 = 0x0803B000U,  /* Base @ of Page 59, 4 Kbytes */
    FLASH_PAGE_60 = 0x0803C000U,  /* Base @ of Page 60, 4 Kbytes */
    FLASH_PAGE_61 = 0x0803D000U,  /* Base @ of Page 61, 4 Kbytes */
    FLASH_PAGE_62 = 0x0803E000U,  /* Base @ of Page 62, 4 Kbytes */
    FLASH_PAGE_63 = 0x0803F000U,  /* Base @ of Page 63, 4 Kbytes */
    FLASH_PAGE_64 = 0x08040000U,  /* Base @ of Page 64, 4 Kbytes */
    FLASH_PAGE_65 = 0x08041000U,  /* Base @ of Page 65, 4 Kbytes */
    FLASH_PAGE_66 = 0x08042000U,  /* Base @ of Page 66, 4 Kbytes */
    FLASH_PAGE_67 = 0x08043000U,  /* Base @ of Page 67, 4 Kbytes */
    FLASH_PAGE_68 = 0x08044000U,  /* Base @ of Page 68, 4 Kbytes */
    FLASH_PAGE_69 = 0x08045000U,  /* Base @ of Page 69, 4 Kbytes */
    FLASH_PAGE_70 = 0x08046000U,  /* Base @ of Page 70, 4 Kbytes */
    FLASH_PAGE_71 = 0x08047000U,  /* Base @ of Page 71, 4 Kbytes */
    FLASH_PAGE_72 = 0x08048000U,  /* Base @ of Page 72, 4 Kbytes */
    FLASH_PAGE_73 = 0x08049000U,  /* Base @ of Page 73, 4 Kbytes */
    FLASH_PAGE_74 = 0x0804A000U,  /* Base @ of Page 74, 4 Kbytes */
    FLASH_PAGE_75 = 0x0804B000U,  /* Base @ of Page 75, 4 Kbytes */
    FLASH_PAGE_76 = 0x0804C000U,  /* Base @ of Page 76, 4 Kbytes */
    FLASH_PAGE_77 = 0x0804D000U,  /* Base @ of Page 77, 4 Kbytes */
    FLASH_PAGE_78 = 0x0804E000U,  /* Base @ of Page 78, 4 Kbytes */
    FLASH_PAGE_79 = 0x0804F000U,  /* Base @ of Page 79, 4 Kbytes */
    FLASH_PAGE_80 = 0x08050000U,  /* Base @ of Page 80, 4 Kbytes */
    FLASH_PAGE_81 = 0x08051000U,  /* Base @ of Page 81, 4 Kbytes */
    FLASH_PAGE_82 = 0x08052000U,  /* Base @ of Page 82, 4 Kbytes */
    FLASH_PAGE_83 = 0x08053000U,  /* Base @ of Page 83, 4 Kbytes */
    FLASH_PAGE_84 = 0x08054000U,  /* Base @ of Page 84, 4 Kbytes */
    FLASH_PAGE_85 = 0x08055000U,  /* Base @ of Page 85, 4 Kbytes */
    FLASH_PAGE_86 = 0x08056000U,  /* Base @ of Page 86, 4 Kbytes */
    FLASH_PAGE_87 = 0x08057000U,  /* Base @ of Page 87, 4 Kbytes */
    FLASH_PAGE_88 = 0x08058000U,  /* Base @ of Page 88, 4 Kbytes */
    FLASH_PAGE_89 = 0x08059000U,  /* Base @ of Page 89, 4 Kbytes */
    FLASH_PAGE_90 = 0x0805A000U,  /* Base @ of Page 90, 4 Kbytes */
    FLASH_PAGE_91 = 0x0805B000U,  /* Base @ of Page 91, 4 Kbytes */
    FLASH_PAGE_92 = 0x0805C000U,  /* Base @ of Page 92, 4 Kbytes */
    FLASH_PAGE_93 = 0x0805D000U,  /* Base @ of Page 93, 4 Kbytes */
    FLASH_PAGE_94 = 0x0805E000U,  /* Base @ of Page 94, 4 Kbytes */
    FLASH_PAGE_95 = 0x0805F000U,  /* Base @ of Page 95, 4 Kbytes */
    FLASH_PAGE_96 = 0x08060000U,  /* Base @ of Page 96, 4 Kbytes */
    FLASH_PAGE_97 = 0x08061000U,  /* Base @ of Page 97, 4 Kbytes */
    FLASH_PAGE_98 = 0x08062000U,  /* Base @ of Page 98, 4 Kbytes */
    FLASH_PAGE_99 = 0x08063000U,  /* Base @ of Page 99, 4 Kbytes */
    FLASH_PAGE_100 = 0x08064000U, /* Base @ of Page 100, 4 Kbytes */
    FLASH_PAGE_101 = 0x08065000U, /* Base @ of Page 101, 4 Kbytes */
    FLASH_PAGE_102 = 0x08066000U, /* Base @ of Page 102, 4 Kbytes */
    FLASH_PAGE_103 = 0x08067000U, /* Base @ of Page 103, 4 Kbytes */
    FLASH_PAGE_104 = 0x08068000U, /* Base @ of Page 104, 4 Kbytes */
    FLASH_PAGE_105 = 0x08069000U, /* Base @ of Page 105, 4 Kbytes */
    FLASH_PAGE_106 = 0x0806A000U, /* Base @ of Page 106, 4 Kbytes */
    FLASH_PAGE_107 = 0x0806B000U, /* Base @ of Page 107, 4 Kbytes */
    FLASH_PAGE_108 = 0x0806C000U, /* Base @ of Page 108, 4 Kbytes */
    FLASH_PAGE_109 = 0x0806D000U, /* Base @ of Page 109, 4 Kbytes */
    FLASH_PAGE_110 = 0x0806E000U, /* Base @ of Page 110, 4 Kbytes */
    FLASH_PAGE_111 = 0x0806F000U, /* Base @ of Page 111, 4 Kbytes */
    FLASH_PAGE_112 = 0x08070000U, /* Base @ of Page 112, 4 Kbytes */
    FLASH_PAGE_113 = 0x08071000U, /* Base @ of Page 113, 4 Kbytes */
    FLASH_PAGE_114 = 0x08072000U, /* Base @ of Page 114, 4 Kbytes */
    FLASH_PAGE_115 = 0x08073000U, /* Base @ of Page 115, 4 Kbytes */
    FLASH_PAGE_116 = 0x08074000U, /* Base @ of Page 116, 4 Kbytes */
    FLASH_PAGE_117 = 0x08075000U, /* Base @ of Page 117, 4 Kbytes */
    FLASH_PAGE_118 = 0x08076000U, /* Base @ of Page 118, 4 Kbytes */
    FLASH_PAGE_119 = 0x08077000U, /* Base @ of Page 119, 4 Kbytes */
    FLASH_PAGE_120 = 0x08078000U, /* Base @ of Page 120, 4 Kbytes */
    FLASH_PAGE_121 = 0x08079000U, /* Base @ of Page 121, 4 Kbytes */
    FLASH_PAGE_122 = 0x0807A000U, /* Base @ of Page 122, 4 Kbytes */
    FLASH_PAGE_123 = 0x0807B000U, /* Base @ of Page 123, 4 Kbytes */
    FLASH_PAGE_124 = 0x0807C000U, /* Base @ of Page 124, 4 Kbytes */
    FLASH_PAGE_125 = 0x0807D000U, /* Base @ of Page 125, 4 Kbytes */
    FLASH_PAGE_126 = 0x0807E000U, /* Base @ of Page 126, 4 Kbytes */
    FLASH_PAGE_127 = 0x0807F000U, /* Base @ of Page 127, 4 Kbytes */
    FLASH_PAGE_128 = 0x08080000U, /* Base @ of Page 128, 4 Kbytes */
    FLASH_PAGE_129 = 0x08081000U, /* Base @ of Page 129, 4 Kbytes */
    FLASH_PAGE_130 = 0x08082000U, /* Base @ of Page 130, 4 Kbytes */
    FLASH_PAGE_131 = 0x08083000U, /* Base @ of Page 131, 4 Kbytes */
    FLASH_PAGE_132 = 0x08084000U, /* Base @ of Page 132, 4 Kbytes */
    FLASH_PAGE_133 = 0x08085000U, /* Base @ of Page 133, 4 Kbytes */
    FLASH_PAGE_134 = 0x08086000U, /* Base @ of Page 134, 4 Kbytes */
    FLASH_PAGE_135 = 0x08087000U, /* Base @ of Page 135, 4 Kbytes */
    FLASH_PAGE_136 = 0x08088000U, /* Base @ of Page 136, 4 Kbytes */
    FLASH_PAGE_137 = 0x08089000U, /* Base @ of Page 137, 4 Kbytes */
    FLASH_PAGE_138 = 0x0808A000U, /* Base @ of Page 138, 4 Kbytes */
    FLASH_PAGE_139 = 0x0808B000U, /* Base @ of Page 139, 4 Kbytes */
    FLASH_PAGE_140 = 0x0808C000U, /* Base @ of Page 140, 4 Kbytes */
    FLASH_PAGE_141 = 0x0808D000U, /* Base @ of Page 141, 4 Kbytes */
    FLASH_PAGE_142 = 0x0808E000U, /* Base @ of Page 142, 4 Kbytes */
    FLASH_PAGE_143 = 0x0808F000U, /* Base @ of Page 143, 4 Kbytes */
    FLASH_PAGE_144 = 0x08090000U, /* Base @ of Page 144, 4 Kbytes */
    FLASH_PAGE_145 = 0x08091000U, /* Base @ of Page 145, 4 Kbytes */
    FLASH_PAGE_146 = 0x08092000U, /* Base @ of Page 146, 4 Kbytes */
    FLASH_PAGE_147 = 0x08093000U, /* Base @ of Page 147, 4 Kbytes */
    FLASH_PAGE_148 = 0x08094000U, /* Base @ of Page 148, 4 Kbytes */
    FLASH_PAGE_149 = 0x08095000U, /* Base @ of Page 149, 4 Kbytes */
    FLASH_PAGE_150 = 0x08096000U, /* Base @ of Page 150, 4 Kbytes */
    FLASH_PAGE_151 = 0x08097000U, /* Base @ of Page 151, 4 Kbytes */
    FLASH_PAGE_152 = 0x08098000U, /* Base @ of Page 152, 4 Kbytes */
    FLASH_PAGE_153 = 0x08099000U, /* Base @ of Page 153, 4 Kbytes */
    FLASH_PAGE_154 = 0x0809A000U, /* Base @ of Page 154, 4 Kbytes */
    FLASH_PAGE_155 = 0x0809B000U, /* Base @ of Page 155, 4 Kbytes */
    FLASH_PAGE_156 = 0x0809C000U, /* Base @ of Page 156, 4 Kbytes */
    FLASH_PAGE_157 = 0x0809D000U, /* Base @ of Page 157, 4 Kbytes */
    FLASH_PAGE_158 = 0x0809E000U, /* Base @ of Page 158, 4 Kbytes */
    FLASH_PAGE_159 = 0x0809F000U, /* Base @ of Page 159, 4 Kbytes */
    FLASH_PAGE_160 = 0x080A0000U, /* Base @ of Page 160, 4 Kbytes */
    FLASH_PAGE_161 = 0x080A1000U, /* Base @ of Page 161, 4 Kbytes */
    FLASH_PAGE_162 = 0x080A2000U, /* Base @ of Page 162, 4 Kbytes */
    FLASH_PAGE_163 = 0x080A3000U, /* Base @ of Page 163, 4 Kbytes */
    FLASH_PAGE_164 = 0x080A4000U, /* Base @ of Page 164, 4 Kbytes */
    FLASH_PAGE_165 = 0x080A5000U, /* Base @ of Page 165, 4 Kbytes */
    FLASH_PAGE_166 = 0x080A6000U, /* Base @ of Page 166, 4 Kbytes */
    FLASH_PAGE_167 = 0x080A7000U, /* Base @ of Page 167, 4 Kbytes */
    FLASH_PAGE_168 = 0x080A8000U, /* Base @ of Page 168, 4 Kbytes */
    FLASH_PAGE_169 = 0x080A9000U, /* Base @ of Page 169, 4 Kbytes */
    FLASH_PAGE_170 = 0x080AA000U, /* Base @ of Page 170, 4 Kbytes */
    FLASH_PAGE_171 = 0x080AB000U, /* Base @ of Page 171, 4 Kbytes */
    FLASH_PAGE_172 = 0x080AC000U, /* Base @ of Page 172, 4 Kbytes */
    FLASH_PAGE_173 = 0x080AD000U, /* Base @ of Page 173, 4 Kbytes */
    FLASH_PAGE_174 = 0x080AE000U, /* Base @ of Page 174, 4 Kbytes */
    FLASH_PAGE_175 = 0x080AF000U, /* Base @ of Page 175, 4 Kbytes */
    FLASH_PAGE_176 = 0x080B0000U, /* Base @ of Page 176, 4 Kbytes */
    FLASH_PAGE_177 = 0x080B1000U, /* Base @ of Page 177, 4 Kbytes */
    FLASH_PAGE_178 = 0x080B2000U, /* Base @ of Page 178, 4 Kbytes */
    FLASH_PAGE_179 = 0x080B3000U, /* Base @ of Page 179, 4 Kbytes */
    FLASH_PAGE_180 = 0x080B4000U, /* Base @ of Page 180, 4 Kbytes */
    FLASH_PAGE_181 = 0x080B5000U, /* Base @ of Page 181, 4 Kbytes */
    FLASH_PAGE_182 = 0x080B6000U, /* Base @ of Page 182, 4 Kbytes */
    FLASH_PAGE_183 = 0x080B7000U, /* Base @ of Page 183, 4 Kbytes */
    FLASH_PAGE_184 = 0x080B8000U, /* Base @ of Page 184, 4 Kbytes */
    FLASH_PAGE_185 = 0x080B9000U, /* Base @ of Page 185, 4 Kbytes */
    FLASH_PAGE_186 = 0x080BA000U, /* Base @ of Page 186, 4 Kbytes */
    FLASH_PAGE_187 = 0x080BB000U, /* Base @ of Page 187, 4 Kbytes */
    FLASH_PAGE_188 = 0x080BC000U, /* Base @ of Page 188, 4 Kbytes */
    FLASH_PAGE_189 = 0x080BD000U, /* Base @ of Page 189, 4 Kbytes */
    FLASH_PAGE_190 = 0x080BE000U, /* Base @ of Page 190, 4 Kbytes */
    FLASH_PAGE_191 = 0x080BF000U, /* Base @ of Page 191, 4 Kbytes */
    FLASH_PAGE_192 = 0x080C0000U, /* Base @ of Page 192, 4 Kbytes */
    FLASH_PAGE_193 = 0x080C1000U, /* Base @ of Page 193, 4 Kbytes */
    FLASH_PAGE_194 = 0x080C2000U, /* Base @ of Page 194, 4 Kbytes */
    FLASH_PAGE_195 = 0x080C3000U, /* Base @ of Page 195, 4 Kbytes */
    FLASH_PAGE_196 = 0x080C4000U, /* Base @ of Page 196, 4 Kbytes */
    FLASH_PAGE_197 = 0x080C5000U, /* Base @ of Page 197, 4 Kbytes */
    FLASH_PAGE_198 = 0x080C6000U, /* Base @ of Page 198, 4 Kbytes */
    FLASH_PAGE_199 = 0x080C7000U, /* Base @ of Page 199, 4 Kbytes */
    FLASH_PAGE_200 = 0x080C8000U, /* Base @ of Page 200, 4 Kbytes */
    FLASH_PAGE_201 = 0x080C9000U, /* Base @ of Page 201, 4 Kbytes */
    FLASH_PAGE_202 = 0x080CA000U, /* Base @ of Page 202, 4 Kbytes */
    FLASH_PAGE_203 = 0x080CB000U, /* Base @ of Page 203, 4 Kbytes */
    FLASH_PAGE_204 = 0x080CC000U, /* Base @ of Page 204, 4 Kbytes */
    FLASH_PAGE_205 = 0x080CD000U, /* Base @ of Page 205, 4 Kbytes */
    FLASH_PAGE_206 = 0x080CE000U, /* Base @ of Page 206, 4 Kbytes */
    FLASH_PAGE_207 = 0x080CF000U, /* Base @ of Page 207, 4 Kbytes */
    FLASH_PAGE_208 = 0x080D0000U, /* Base @ of Page 208, 4 Kbytes */
    FLASH_PAGE_209 = 0x080D1000U, /* Base @ of Page 209, 4 Kbytes */
    FLASH_PAGE_210 = 0x080D2000U, /* Base @ of Page 210, 4 Kbytes */
    FLASH_PAGE_211 = 0x080D3000U, /* Base @ of Page 211, 4 Kbytes */
    FLASH_PAGE_212 = 0x080D4000U, /* Base @ of Page 212, 4 Kbytes */
    FLASH_PAGE_213 = 0x080D5000U, /* Base @ of Page 213, 4 Kbytes */
    FLASH_PAGE_214 = 0x080D6000U, /* Base @ of Page 214, 4 Kbytes */
    FLASH_PAGE_215 = 0x080D7000U, /* Base @ of Page 215, 4 Kbytes */
    FLASH_PAGE_216 = 0x080D8000U, /* Base @ of Page 216, 4 Kbytes */
    FLASH_PAGE_217 = 0x080D9000U, /* Base @ of Page 217, 4 Kbytes */
    FLASH_PAGE_218 = 0x080DA000U, /* Base @ of Page 218, 4 Kbytes */
    FLASH_PAGE_219 = 0x080DB000U, /* Base @ of Page 219, 4 Kbytes */
    FLASH_PAGE_220 = 0x080DC000U, /* Base @ of Page 220, 4 Kbytes */
    FLASH_PAGE_221 = 0x080DD000U, /* Base @ of Page 221, 4 Kbytes */
    FLASH_PAGE_222 = 0x080DE000U, /* Base @ of Page 222, 4 Kbytes */
    FLASH_PAGE_223 = 0x080DF000U, /* Base @ of Page 223, 4 Kbytes */
    FLASH_PAGE_224 = 0x080E0000U, /* Base @ of Page 224, 4 Kbytes */
    FLASH_PAGE_225 = 0x080E1000U, /* Base @ of Page 225, 4 Kbytes */
    FLASH_PAGE_226 = 0x080E2000U, /* Base @ of Page 226, 4 Kbytes */
    FLASH_PAGE_227 = 0x080E3000U, /* Base @ of Page 227, 4 Kbytes */
    FLASH_PAGE_228 = 0x080E4000U, /* Base @ of Page 228, 4 Kbytes */
    FLASH_PAGE_229 = 0x080E5000U, /* Base @ of Page 229, 4 Kbytes */
    FLASH_PAGE_230 = 0x080E6000U, /* Base @ of Page 230, 4 Kbytes */
    // Start of CPU2 Secure Memory:
    FLASH_PAGE_231 = 0x080E7000U, /* Base @ of Page 231, 4 Kbytes */
    FLASH_PAGE_232 = 0x080E8000U, /* Base @ of Page 232, 4 Kbytes */
    FLASH_PAGE_233 = 0x080E9000U, /* Base @ of Page 233, 4 Kbytes */
    FLASH_PAGE_234 = 0x080EA000U, /* Base @ of Page 234, 4 Kbytes */
    FLASH_PAGE_235 = 0x080EB000U, /* Base @ of Page 235, 4 Kbytes */
    FLASH_PAGE_236 = 0x080EC000U, /* Base @ of Page 236, 4 Kbytes */
    FLASH_PAGE_237 = 0x080ED000U, /* Base @ of Page 237, 4 Kbytes */
    FLASH_PAGE_238 = 0x080EE000U, /* Base @ of Page 238, 4 Kbytes */
    FLASH_PAGE_239 = 0x080EF000U, /* Base @ of Page 239, 4 Kbytes */
    FLASH_PAGE_240 = 0x080F0000U, /* Base @ of Page 240, 4 Kbytes */
    FLASH_PAGE_241 = 0x080F1000U, /* Base @ of Page 241, 4 Kbytes */
    FLASH_PAGE_242 = 0x080F2000U, /* Base @ of Page 242, 4 Kbytes */
    FLASH_PAGE_243 = 0x080F3000U, /* Base @ of Page 243, 4 Kbytes */
    FLASH_PAGE_244 = 0x080F4000U, /* Base @ of Page 244, 4 Kbytes */
    FLASH_PAGE_245 = 0x080F5000U, /* Base @ of Page 245, 4 Kbytes */
    FLASH_PAGE_246 = 0x080F6000U, /* Base @ of Page 246, 4 Kbytes */
    FLASH_PAGE_247 = 0x080F7000U, /* Base @ of Page 247, 4 Kbytes */
    FLASH_PAGE_248 = 0x080F8000U, /* Base @ of Page 248, 4 Kbytes */
    FLASH_PAGE_249 = 0x080F9000U, /* Base @ of Page 249, 4 Kbytes */
    FLASH_PAGE_250 = 0x080FA000U, /* Base @ of Page 250, 4 Kbytes */
    FLASH_PAGE_251 = 0x080FB000U, /* Base @ of Page 251, 4 Kbytes */
    FLASH_PAGE_252 = 0x080FC000U, /* Base @ of Page 252, 4 Kbytes */
    FLASH_PAGE_253 = 0x080FD000U, /* Base @ of Page 253, 4 Kbytes */
    FLASH_PAGE_254 = 0x080FE000U, /* Base @ of Page 254, 4 Kbytes */
    FLASH_PAGE_255 = 0x080FF000U, /* Base @ of Page 255, 4 Kbytes */
} flash_page;

#define FLASH_KEY1 0x45670123U /*!< Flash key1 */
#define FLASH_KEY2                                                             \
    0xCDEF89ABU /*!< Flash key2: used with FLASH_KEY1                          \
                     to unlock the FLASH registers access */

#define FLASH_FLAG_EOP FLASH_SR_EOP     /*!< FLASH End of operation flag */
#define FLASH_FLAG_OPERR FLASH_SR_OPERR /*!< FLASH Operation error flag */
#define FLASH_FLAG_PROGERR                                                     \
    FLASH_SR_PROGERR /*!< FLASH Programming error flag                         \
                      */
#define FLASH_FLAG_WRPERR                                                      \
    FLASH_SR_WRPERR /*!< FLASH Write protection error flag */
#define FLASH_FLAG_PGAERR                                                      \
    FLASH_SR_PGAERR /*!< FLASH Programming alignment error flag */
#define FLASH_FLAG_SIZERR FLASH_SR_SIZERR /*!< FLASH Size error flag  */
#define FLASH_FLAG_PGSERR                                                      \
    FLASH_SR_PGSERR /*!< FLASH Programming sequence error flag */
#define FLASH_FLAG_MISERR                                                      \
    FLASH_SR_MISERR /*!< FLASH Fast programming data miss error flag */
#define FLASH_FLAG_FASTERR                                                     \
    FLASH_SR_FASTERR /*!< FLASH Fast programming error flag */
#define FLASH_FLAG_OPTNV                                                       \
    FLASH_SR_OPTNV /*!< FLASH User Option OPTVAL indication */
#define FLASH_FLAG_RDERR FLASH_SR_RDERR /*!< FLASH PCROP read error flag */
#define FLASH_FLAG_OPTVERR                                                     \
    FLASH_SR_OPTVERR                /*!< FLASH Option validity error flag  */
#define FLASH_FLAG_BSY FLASH_SR_BSY /*!< FLASH Busy flag */
#define FLASH_FLAG_CFGBSY                                                      \
    FLASH_SR_CFGBSY /*!< FLASH Programming/erase configuration busy */
#define FLASH_FLAG_PESD                                                        \
    FLASH_SR_PESD /*!< FLASH Programming/erase operation suspended */
#define FLASH_FLAG_ECCC FLASH_ECCR_ECCC /*!< FLASH ECC correction */
#define FLASH_FLAG_ECCD FLASH_ECCR_ECCD /*!< FLASH ECC detection */

#define FLASH_FLAG_SR_ERRORS                                                   \
    (FLASH_FLAG_OPERR | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR |               \
     FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_PGSERR |               \
     FLASH_FLAG_MISERR | FLASH_FLAG_FASTERR | FLASH_FLAG_RDERR |               \
     FLASH_FLAG_OPTVERR) /*!< All SR error flags */

#define FLASH_FLAG_ECCR_ERRORS (FLASH_FLAG_ECCC | FLASH_FLAG_ECCD)

#define FLASH_FLAG_ALL_ERRORS (FLASH_FLAG_SR_ERRORS | FLASH_FLAG_ECCR_ERRORS)

#define FLASH_CLEAR_FLAG(__FLAG__)                                             \
    do {                                                                       \
        if (((__FLAG__) & (FLASH_FLAG_ECCR_ERRORS)) != 0U) {                   \
            SET_BIT(FLASH->ECCR, ((__FLAG__) & (FLASH_FLAG_ECCR_ERRORS)));     \
        }                                                                      \
        if (((__FLAG__) & ~(FLASH_FLAG_ECCR_ERRORS)) != 0U) {                  \
            WRITE_REG(FLASH->SR, ((__FLAG__) & ~(FLASH_FLAG_ECCR_ERRORS)));    \
        }                                                                      \
    } while (0)

#define FLASH_GET_FLAG(__FLAG__)                                               \
    ((((__FLAG__) & (FLASH_FLAG_ECCR_ERRORS)) != 0U)                           \
         ? (READ_BIT(FLASH->ECCR, (__FLAG__)) == (__FLAG__))                   \
         : (READ_BIT(FLASH->SR, (__FLAG__)) == (__FLAG__)))

#define HAL_FLASH_ERROR_NONE 0x00000000U
#define HAL_FLASH_ERROR_OP FLASH_FLAG_OPERR
#define HAL_FLASH_ERROR_PROG FLASH_FLAG_PROGERR
#define HAL_FLASH_ERROR_WRP FLASH_FLAG_WRPERR
#define HAL_FLASH_ERROR_PGA FLASH_FLAG_PGAERR
#define HAL_FLASH_ERROR_SIZ FLASH_FLAG_SIZERR
#define HAL_FLASH_ERROR_PGS FLASH_FLAG_PGSERR
#define HAL_FLASH_ERROR_MIS FLASH_FLAG_MISERR
#define HAL_FLASH_ERROR_FAST FLASH_FLAG_FASTERR
#define HAL_FLASH_ERROR_RD FLASH_FLAG_RDERR
#define HAL_FLASH_ERROR_OPTV FLASH_FLAG_OPTVERR

#ifndef FLASH_TIMEOUT_VALUE
#define FLASH_TIMEOUT_VALUE 1000U
#endif // FLASH_TIMEOUT_VALUE

#define FLASH_PAGE_SIZE 0x00001000U /*!< FLASH Page Size, 4 KBytes */
#define FLASH_PAGE_NB (FLASH_SIZE / FLASH_PAGE_SIZE)

#define IS_ADDR_ALIGNED_64BITS(__VALUE__) (((__VALUE__) & 0x7U) == (0x00UL))

#define IS_FLASH_PROGRAM_MAIN_MEM_ADDRESS(__VALUE__)                           \
    (((__VALUE__) >= FLASH_BASE) &&                                            \
     ((__VALUE__) <= (FLASH_BASE + FLASH_SIZE - 8UL)) &&                       \
     (((__VALUE__) % 8UL) == 0UL))

#define IS_FLASH_PROGRAM_OTP_ADDRESS(__VALUE__)                                \
    (((__VALUE__) >= OTP_AREA_BASE) &&                                         \
     ((__VALUE__) <= (OTP_AREA_END_ADDR + 1UL - 8UL)) &&                       \
     (((__VALUE__) % 8UL) == 0UL))

#define IS_FLASH_FAST_PROGRAM_ADDRESS(__VALUE__)                               \
    (((__VALUE__) >= FLASH_BASE) &&                                            \
     ((__VALUE__) <= (FLASH_BASE + FLASH_SIZE - 256UL)) &&                     \
     (((__VALUE__) % 256UL) == 0UL))

#define IS_FLASH_PROGRAM_ADDRESS(__VALUE__)                                    \
    (IS_FLASH_PROGRAM_MAIN_MEM_ADDRESS(__VALUE__) ||                           \
     IS_FLASH_PROGRAM_OTP_ADDRESS(__VALUE__))

typedef enum {
    FLASH_TYPEPROGRAM_DOUBLEWORD =
        FLASH_CR_PG, /*!< Program a double-word (64-bit) at a specified
                        address.*/
    FLASH_TYPEPROGRAM_FAST =
        FLASH_CR_FSTPG, /*!< Fast program a 64 row double-word (64-bit) at a
                           specified address. And another 64 row double-word
                           (64-bit) will be programmed */
} flash_typeprogram;

typedef enum {
    FLASH_LATENCY_ZERO_WAIT = 0U,
    FLASH_LATENCY_ONE_WAIT = 1U,
    FLASH_LATENCT_TWO_WAIT = 2U,
    FLASH_LATENCY_THREE_WAIT = 3U,
} flash_latency;

typedef struct {

    bool lock;

    uint32_t error_code;
    uint32_t procedure_ongoing;
    uint32_t address;
    uint32_t page;
    uint32_t amount_to_erase;

} flash_state_t;

void flash_enable_prefetch();

void flash_select_latency(flash_latency latency);
flash_latency flash_get_latency();

static inline uint32_t flash_get_page(uint32_t address) {
    return (address - FLASH_BASE) / FLASH_PAGE_SIZE;
}

flash_state_t *flash_get_state();

hal_err flash_wait_for_last_operation(uint32_t timeout);

hal_err flash_erase(flash_page start_page, uint32_t page_amount,
                    uint32_t *page_error);

hal_err flash_program(flash_typeprogram typeprogram, uint32_t address,
                      uint64_t data);

hal_err flash_unlock();
hal_err flash_lock();

#endif // HAL_FLASH_H
