MAIN: add      r3, r4
LOOP: prn      #48
           mcro     a_mc
           cmp      r4, #-6
           bne      &END
           mcroend
           lea      r4, r6
           inc      r6
           mov      r3, r4
           sub      r1, r4
           bne      END
           a_mc
           dec      K
           jmp      &LOOP
END:       stop
