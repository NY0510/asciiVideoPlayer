ffmpeg -i $1.webm -vf "fps=15, scale=iw/4:-1" frames/frame_%d.bmp
#yt-dlp --format 244 
#yt-dlp --list-formats 
