# Hand Gesture Detection System — Command Reference

Total registered commands: **20**

Every command has a fixed numeric code starting from zero (suitable for simple output like serial/Arduino) as well as a human-readable text identifier.

| Code | Command ID | Label | Label (alt) | Category | Description | Parameters |
|---|---|---|---|---|---|---|
| **0** | `finger.swipe.down` | Finger Swipe Down | Finger Swipe Down | gesture | While pointing, the index fingertip moves downward | `speed`: float |
| **1** | `finger.swipe.up` | Finger Swipe Up | Finger Swipe Up | gesture | While pointing, the index fingertip moves upward | `speed`: float |
| **2** | `finger.swipe.left` | Finger Swipe Left | Finger Swipe Left | gesture | While pointing, the index fingertip moves left | `speed`: float |
| **3** | `finger.swipe.right` | Finger Swipe Right | Finger Swipe Right | gesture | While pointing, the index fingertip moves right | `speed`: float |
| **4** | `hand.fist` | Fist | Fist | gesture | All four fingers (index through pinky) fully curl in | - |
| **5** | `hand.open` | Open Hand | Open Hand | gesture | The hand relaxes out of a fist back into an open pose | - |
| **6** | `hand.move.left` | Hand Move Left | Hand Move Left | movement | The palm center moves to the left | `speed`: float |
| **7** | `hand.move.right` | Hand Move Right | Hand Move Right | movement | The palm center moves to the right | `speed`: float |
| **8** | `hand.move.up` | Hand Move Up | Hand Move Up | movement | The palm center moves upward | `speed`: float |
| **9** | `hand.move.down` | Hand Move Down | Hand Move Down | movement | The palm center moves downward | `speed`: float |
| **10** | `hand.move.forward` | Hand Move Forward | Hand Move Forward | movement | The hand moves closer to the camera (based on the hand appearing larger) | `speed`: float |
| **11** | `hand.move.backward` | Hand Move Backward | Hand Move Backward | movement | The hand moves away from the camera (based on the hand appearing smaller) | `speed`: float |
| **12** | `hand.pinch.start` | Pinch Start | Pinch Start | gesture | The thumb tip and index tip come close together | - |
| **13** | `hand.pinch.rub` | Pinch Rub | Pinch Rub | gesture | While the two fingers are close together, they move back and forth (rubbing) | `intensity`: float |
| **14** | `hand.pinch.end` | Pinch End | Pinch End | gesture |  | - |
| **15** | `hand.thumb.show` | Thumb Shown | Thumb Shown | gesture | Only the thumb is extended, with the other four fingers curled in | `direction`: str |
| **16** | `hand.thumb.hide` | Thumb Hidden | Thumb Hidden | gesture |  | - |
| **17** | `hand.open_palm.swipe_right` | Open Palm Swipe Right | Open Palm Swipe Right | gesture | All five fingers are extended and the whole hand moves to the right | `speed`: float |
| **18** | `hand.rig.frame` | Full Hand Rig Frame | Full Hand Rig Frame | rig | The position of every one of the 21 hand landmarks (exactly matching MediaPipe's raw output) along with each finger's curl angle and extended/curled state, emitted on every frame (or at a capped rate). For connecting to a 3D model/character rig, robotics, or any consumer that needs the raw data. | `landmarks`: Array of 21 normalized [x, y, z] points, in standard MediaPipe order, `landmarks_named`: The same 21 points keyed by joint name (e.g. INDEX_TIP), `finger_states`: Boolean dictionary of extended/curled for each finger, `finger_curl_angles`: Each finger's curl angle in degrees (180 = fully straight), `palm_center`: [x, y, z] center of the palm, `hand_size`: Approximate hand size - usable to estimate depth/distance from the camera |
| **19** | `hands.pair.frame` | Two-Hand Frame Shape | Two-Hand Frame Shape | pair | When both hands are seen at the same time, a quadrilateral is built between reference points on the two hands (wrist and index finger base) that changes shape/size/angle as the hands move - exactly the 'two-hand framing' pattern seen in MediaPipe demos. | `corners`: Four corner points in order [bottom-left, top-left, top-right, bottom-right], each [x, y], `width`: Normalized distance between the two wrists (e.g. usable for zoom control), `height`: Average distance from wrist to index finger base for each hand, `center`: [x, y] center of the quadrilateral, `angle_deg`: Tilt angle of the line connecting the two hands, in degrees, `area`: Approximate area of the quadrilateral (normalized, between 0 and 1) |
