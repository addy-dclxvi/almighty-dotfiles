#! /bin/bash
	MUSIC_DIR=/media/addy/Media/musik/
	COVER=/tmp/cover.png
    BACKGROUND="/home/addy/.popup/cover-top.png"
    
{	album="$(mpc --format %album% current)"
    file="$(mpc --format %file% current)"
    album_dir="${file%/*}"
    [[ -z "$album_dir" ]] && exit 1
    album_dir="$MUSIC_DIR/$album_dir"

    covers="$(find "$album_dir" -type d -exec find {} -maxdepth 1 -type f -iregex ".*/.*\(${album}\|cover\|folder\|artwork\|front\).*[.]\(jpe?g\|png\|gif\|bmp\)" \; )"
    src="$(echo -n "$covers" | head -n1)"
    rm -f "$COVER" 
    convert "$src" -resize 150 "$COVER"
    n30f -x 1095 -y 45 -d ${BACKGROUND}
    n30f -x 1120 -y 70 -d ${COVER}
} &
