"../atlasgen-debug.exe" non_unique/non_unique.json -non-unique-textures
"../atlasgen-debug.exe" bug-48/bug-48.json 
"../atlasgen-debug.exe" bug-47/bug-47.json 
"../atlasgen-debug.exe" merge1/non_unique_1.json merge1/non_unique_2.json -non-unique-textures
"../atlasgen-debug.exe" merge2/non_unique_atlas.json merge2/non_unique_atlas2.json merge2/itemicons.json -non-unique-textures
"../atlasgen-debug.exe" srgba/srgba.json -srgba 
"../atlasgen-debug.exe" 1pix/1pix.json --add-pixel --full-search
"../atlasgen-debug.exe" 1pix/1pix-one.json --add-pixel --full-search
"../atlasgen-debug.exe" 1pix/1pix-one-growing.json --add-pixel
"../atlasgen-debug.exe" 1pix/1pix-growing.json --add-pixel
"../atlasgen-debug.exe" srgba/srgba.json -srgba -write-to-tar srgba/srgba.tar
"../atlasgen-debug.exe" no-mipmaps/1pix.json  --add-pixel -srgba -no-mipmaps