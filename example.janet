(import ./build/nunchuk :as nunchuk)
(nunchuk/init)
(while true
  (nunchuk/update)
  (print (nunchuk/jx))
  (os/sleep 0.1))