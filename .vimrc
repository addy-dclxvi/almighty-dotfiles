"                                 ___     
"        ___        ___          /__/\    
"       /__/\      /  /\        |  |::\   
"       \  \:\    /  /:/        |  |:|:\  
"        \  \:\  /__/::\      __|__|:|\:\ 
"    ___  \__\:\ \__\/\:\__  /__/::::| \:\
"   /__/\ |  |:|    \  \:\/\ \  \:\~~\__\/
"   \  \:\|  |:|     \__\::/  \  \:\      
"    \  \:\__|:|     /__/:/    \  \:\     
"     \__\::::/      \__\/      \  \:\    
"         ~~~~                   \__\/    

 " Set compatibility to Vim only.
set nocompatible

"Always show current position
set ruler

" Turn on syntax highlighting.
syntax on

" Turn off modelines
set modelines=0

" Uncomment below to set the max textwidth. Use a value corresponding to the width of your screen.
" set textwidth=80
set formatoptions=tcqrn1
set tabstop=4
set shiftwidth=4
set softtabstop=4
set expandtab
set noshiftround

" Ignore case when searching
set ignorecase

" When searching try to be smart about cases 
set smartcase

" Don't redraw while executing macros (good performance config)
set lazyredraw

" For regular expressions turn magic on
set magic

" Display 5 lines above/below the cursor when scrolling with a mouse.
set scrolloff=5
" Fixes common backspace problems
set backspace=indent,eol,start

" Display options
set showmode
set showcmd
set cmdheight=1

" Highlight matching pairs of brackets. Use the '%' character to jump between them.
set matchpairs+=<:>

" Display different types of white spaces.
"set list
"set listchars=tab:›\ ,trail:•,extends:#,nbsp:.

" Show line numbers
set number
highlight LineNr ctermfg=black

" Set status line display
set laststatus=2
hi StatusLine ctermfg=white ctermbg=red cterm=NONE
hi StatusLineNC ctermfg=black ctermbg=black cterm=NONE
hi User1 ctermfg=red ctermbg=black
hi User2 ctermfg=black ctermbg=magenta
hi User3 ctermfg=magenta ctermbg=black
hi User4 ctermfg=black ctermbg=black
hi User5 ctermfg=blue ctermbg=black
hi User6 ctermfg=black ctermbg=blue
hi User7 ctermfg=cyan ctermbg=black
hi User8 ctermfg=black ctermbg=cyan
set statusline=\ \                 " Padding
set statusline+=%F                  " Path to the file
set statusline+=\ %1*%2*\         " Separator
set statusline+=%y                  " File type
set statusline+=\ %3*%4*\         " Separator
set statusline+=%=                  " Switch to right-side
set statusline+=\ %5*%6*\         " Separator
set statusline+=%p%%                " Line percent
set statusline+=\ %7*%8*\         " Separator
set statusline+=%l/%L               " Current line
set statusline+=\ \                 " Padding

" Encoding
set encoding=utf-8

" Highlight matching search patterns
set hlsearch

" Enable incremental search
set incsearch

" Include matching uppercase words with lowercase search term
set ignorecase

" Include only uppercase words with uppercase search term
set smartcase

" Store info from no more than 100 files at a time, 9999 lines of text
" 100kb of data. Useful for copying large amounts of data between files.
set viminfo='100,<9999,s100
