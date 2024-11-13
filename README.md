[![Build Status](https://github.com/rgerganov/ggtag/workflows/CI/badge.svg)](https://github.com/rgerganov/ggtag/actions)
# Overview

`ggtag` is an e-paper tag that can be programmed with sound using the [ggwave](https://github.com/ggerganov/ggwave) library.
It is based on the RP2040 microcontroller and has 3.52" e-paper display.
Additionally, ggtag supports USB serial programming and allows emulation of 125kHz RFID tags (ASK and FSK).

[![CrowdSupply](https://www.crowdsupply.com/_marvin/images/crowd-supply-logo-dark.png)](https://www.crowdsupply.com/eurolan-ltd/ggtag) 

__The GGtag's CrowdSupply campaign has started, you can order one [here](https://www.crowdsupply.com/eurolan-ltd/ggtag)!__

<table>
  <tr>
    <td>
      <img src="https://github.com/rgerganov/ggtag/assets/271616/f1cfaeb3-3cd3-4a6a-91b8-00ee64586646"><img>
    </td>
    <td>
      <img src="https://github.com/rgerganov/ggtag/assets/271616/3d7d9585-6011-4011-8473-a00e232abf33"></img>
    </td>
  </tr>
</table>

Some example tags:
 * [demo](https://ggtag.io/?i=%5Cr18%2C15%2C82%2C82%5Cq22%2C19%2C3%2Chttps%3A%2F%2Fggtag.io%5CI273%2C15%2C70%2C70%2C0%2Chttps%3A%2F%2Fggtag.io%2Fggtag-180x180.png%5Ct139%2C118%2C5%2Cggtag%5Ct58%2C149%2C3%2Cprogrammable%20e-paper%20tag%5Ca58%2C200%2C20%2Clink%5Ct86%2C203%2C4%2Chttps%3A%2F%2Fggtag.io)
 * [octocat](https://ggtag.io/?i=%5Cr10%2C25%2C110%2C110%5CI15%2C30%2C100%2C100%2C0%2Chttps%3A%2F%2Favatars.githubusercontent.com%2Fu%2F583231%5Ct140%2C50%2C5%2CThe%20Octocat%5Ct140%2C80%2C2%2Cgithub.com%2Foctocat%5Ca13%2C156%2C16%2Cmap-marker-alt%5Ct33%2C158%2C2%2CSan%20Francisco%5Ca13%2C183%2C16%2Cbuilding%5Ct33%2C185%2C2%2C%40github%5Ca180%2C154%2C16%2Clink%5Ct202%2C158%2C2%2Chttps%3A%2F%2Fgithub.blog%5Ca180%2C185%2C16%2Cenvelope%5Ct202%2C185%2C2%2Coctocat%40github.com)
 * [mario](https://ggtag.io/?i=%5CI17%2C19%2C0%2C0%2C0%2Chttps%3A%2F%2Fggtag.io%2Fmario.png%5Ct144%2C42%2C5%2CSUPER%20MARIO%5Ct146%2C75%2C4%2Cplumber%5Ca30%2C165%2C23%2Cphone-alt%5Ct68%2C168%2C3%2C08812345%5Ca30%2C208%2C23%2Cenvelope%5Ct68%2C210%2C3%2Cm%40ggtag.io%5Ca215%2C165%2C23%2Cglobe%5Ct248%2C167%2C3%2Cggtag.io%5Ca217%2C203%2C23%2Cmap-marker-alt%5Ct248%2C208%2C3%2CSofia)
 * [wifi](https://ggtag.io/?i=%5Ca40%2C35%2C75%2Cwifi%5Cq215%2C30%2C3%2CWIFI%3AS%3AHotel%20WiFi%3BT%3AWPA%3BP%3A12345678%3B%3B%5Ct40%2C155%2C5%2CSSID%3A%20Hotel%20WiFi%5Ct40%2C185%2C5%2C%20PWD%3A%2012345678)
 * [hello my name is](https://ggtag.io/?i=%5CP0%2C0%2C360%2C239%2C0%2CiVBORw0KGgoAAAANSUhEUgAAAWgAAADvCAYAAADfJ04BAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH6AsMEzgpQIiEWAAAFHxJREFUeJzt3WFoFOkdx%2FFfJC98sWJhUtxcUlgXrRSkJGpEqC1L0kLULT3Q6%2BUs7rVw8bj2hdFEyL0o7d29aME10Rd3tUbQ09J4xHKRGPPiGi%2BoYDnPxCt9oxsSoUl2BZc72QWFCNMXafaym5m4SXZ3nuj3A3%2B43dmZeWYu%2FHz2mWdnymzbtlVE33zzjf75z39qdHRUDx8%2BVCKRUCKR0MTEhBKJhNLpdDF3DwBL5vP55Pf7VV1dLb%2FfL7%2Ffr3Xr1mnDhg1qbGyUz%2Bcr6v7LihHQExMT6u3t1eXLlzU0NKRnz54VehcA4Kny8nKFQiHt3btX4XBY1dXVhd%2BJXSBPnjyxOzs77W3bttmSKIqiXqrasWOH3dnZaT958qRQsWoXJKDPnj1rBwIBz08QRVGU1xUIBOyzZ88WIlqXF9Cff%2F65XVNT4%2FkJoSiKMq1qamrszz%2F%2FvPQBHY%2FH7XA47PkJoCiKMr3C4bAdj8dLE9C3b9%2B2%2FX6%2F5wdNURS1Usrv99u3b98ubkBfuHDBXr16tecHS1EUtdJq9erV9oULFwof0NPT03Z7e7vnB0hRFLXSq7293Z6eni5MQE9PT9v79u3z%2FKAoiqJelNq3b19eIf3cgKbnTFEUVfhqaWlZXkBfuHDB84OgClvRaNT1%2F3c0Gl3SNvv6%2Bly3GYlEPD%2Fmhdo4NTVlB4PBgq1DUYuprq6uhSLYXiUXX375pZqbm90WAwCW6Z133tHNmzddlzsGdCKR0M9%2F%2FnM9ffq0aA0DgJfds2fP9Nprr%2BnBgweOyx0Durm5WYlEopjtAgBopkP8m9%2F8xnHZvIAeGhrSlStXit4oAMCMoaEh9fb2znt%2FXkAfPny4JA0CAHzr97%2F%2F%2FbxbM2cF9Llz53T37t2SNgoAIP3nP%2F%2FR3%2F72t6z3MgH99OlTvffeeyVvFABgxrvvvps1OSMT0KdOnXK9kgiUimVZev%2F99zU8PKxUKiV7Zq6%2BbNtWLBbTJ598osbGRq%2BbuWiWZam5uVnXr19XPB7POq5UKqXh4WG9%2F%2F77sizL66bCQ4lEQqdOnfr2jdkJ0TwJ5eUoU3%2BoYlmW3d3dveCk%2Fbnu379vNzc3L7uNxf6hyuxxpVKpvI%2Btv7%2Ffrqur8%2FxvhfKmduzYkflbWCXNPEPwyy%2B%2FFOCFuro63bp1S01NTXmvs3HjRp0%2BfVrd3d3G9jobGhr04MEDNTU1Lerhort379a1a9d05MiRIrYOpvrXv%2F6liYkJSf8f4nCa3gGUQjAY1OXLl7Vx48Ylrd%2FU1KRz584VtlEFEIlE1Nvbu%2BSnPvt8Ph0%2FflzRaLTALcNKMDvVeZUkXb582dPG4OVkWZYuXbqkysrKZW0nHA4bFWQNDQ368MMPlxzOc7W2tioSiRSgVVhJ%2FvGPf0iSVn3zzTcaGhrytjV4Kb377ruqra11XBaLxXTw4EGVlZWprKxMFRUVOnPmjOu23n77bTU0NBSrqXmzLEvHjh1zDed0Oq3W1lZVVFRkju3gwYOKxWKu2%2Fzzn%2F%2BsYDBYrCbDQENDQ0qn01JPT4%2Fng%2BJU6Wqhi4TF4HaRMBgM2lNTU47rDA8P25ZlOa4XiURc97XQRc5SXSRc6PxOTU25XvyzLGvBi619fX2e%2F%2B1Qpa2enh571ejoqIBS%2B%2B1vf%2Bs4tJFOp3X06FElk0nH9c6fP6%2Fjx487Ltu%2Ff7%2BnPU3LslRfX%2B%2B4LJ1O68CBA7p9%2B7bj8mQyqV%2F%2F%2BtcaGRlxXB4KhYz4hoDSGR0d1aqHDx963Q68hDZt2uT4%2FtDQkAYHBxdcd2BgYObrX47Kykrt3LmzIO1bij179rgO2fz1r3997nElk0mdOHHCcZnP59OuXbuW3UasHA8fPtQq7lqHUgsGg9q6davjsnv37j13%2Fbt377qO2f7whz9cVtuW47XXXnN8P51Oa2BgIK9t3Lx5U%2FF43HFZfX29sVMKUXiJRIKARumtX79ea9ascVz273%2F%2Fe1nbduuZF5tlWaqqqnJcFovF8r7HzdjYmO7cueO4zO%2F3a%2B3atUtuI1aWRCKhVbMTooFSqaqqcp3l8PHHH2f9DNqpHj165DqUUFVV5Ukvc%2B3atfL7%2FY7LJicnXcfUnbh9i1izZo3Wr1%2B%2FpPZh5ZmYmKAHjW8dP348M%2FVrMcX9wwvL7VuEz%2Bdz7aXjxZNIJLTK6WILAMBb6XTa%2BZFXwEr1IozTenmhE2YhoGGUN998c0nDLLP1yiuvaGxszOvDKIp0Oq3JyUmvm4ESIqBRcpOTk47zmFeyx48fuz5oebEXLt1moqRSKY2Pjy%2BpfViZCGiU3Pj4uFKplOMyt7nEpksmk669240bN6qmpiav7Sw0RzyRSOjx48dLbiNWHgIaJVfI3qZJenp6HN9fzK8Ad%2B7c6Xp3v2vXri1quh5WPgIaJZdMJnXt2jXHZbW1tdqzZ8%2BC60ej0cxjoj755BO1tbWpsbFR3%2F%2F%2B94vR3Lwt9CvAfO62Z1mWWlpaHJct5teIeHEQ0PCE2%2F00JOnDDz90DbNgMKj9%2B%2FfL5%2FOptrZWv%2FzlL3Xs2DENDAzozp07nt5QaGxsTH%2F%2F%2B98dl%2Fl8Pl24cEF1dXWOyy3L0rlz51x%2FgJPPPUrw4iGg4YnBwUHX%2B5D7fD719vaqo6Mja7ijublZX331lesQwGJ%2BUl0sH330kWsvurKyMvMoq9zjunXrlsLhsON68Xhchw4dKkp7YTYCGp45dOiQa5j5fD4dPnxYjx49yvzE%2B%2FTp0wveCH%2Bh25SWytjYmNrb212Xzz7KKve4FnrkV3t7%2Bws7dRALI6DhmbGxMR04cKAgU%2B5%2B97vfGTMEsNA9qxfrzTff1Pnz5wuyLaw8BDQ8NTg4qFdffXXJIT37CCnTQqytrU2tra3LOq6f%2FvSnxh0XSouAhucGBwcVCAR09erVRa0Xi8VUX1%2Bvjo6OIrVseTo6OhQIBHTx4sVFBfXFixcVCASM%2BUYA75R73QBAmpl6t2fPHlmWpUOHDikcDquysjLrFp7pdFqJREJ9fX06deqU7t%2B%2F72GL85NMJvXGG29ImrkY%2BOqrr2rLli3zjisWi%2BnKlSs6efKk5%2BPoMEeZZh5QCAAwDEMcAGAoAhoADEVAA4ChCGhghejr68v8uGVqakrBYNDrJqHICGgAMBQBDQCGYpodABiKHjQAGIqANlxDQ4NSqVTm4lAkEpFlWero6FA8Hs%2B8H4%2FHdeTIkcx6%2BXxm9nPDw8OZzwwPD7s%2B0SQajWY%2Bl0ql8r73cu4%2BotGoLMvSkSNHstqXSqXm3WLUaVu5xzV7bG7rFvscOmlubs465tkHDLjdDzof%2BVwkzN3vbMViMXV0dHj%2BUAMsnk2ZWw0NDXYqlbJnXb16Net1ru7ubjsYDNr37993%2FUxfX1%2FWPqLRaGZZKpWyGxoa5rXDsix7eHg487nh4WHbsqy8jiF33a6urqzXudy2HQwG7ampKdf13NpfinPodqxOotHokv4W%2Bvr6MtuYmpqyg8Hgovbrdn4oo8vzBlALVG645CMejz%2F3M5FIxHUfTgGSz2fcKt%2FwmMtp%2B3MDaiG54VmKc7jYNi4lpBcK6Ln%2FyD5P7rqUucUQxwoUi8W0fft2lZWVqaKiQtevX89a7vf7lU6ndfDgwcxnrly5kvWZuU%2FPvnv3rmKxWOZ1fX39vKGCXbt2ZW6WH4%2FH9dFHHy3rGJ7Xvk2bNmW9bmhoUCgUyrw%2Bc%2BaMKioqVFZWptbW1qzPbt269blzhAt9DiUpEolkPRVlZGQks4%2Ft27dnneP9%2B%2FcXbB6zZVmqr6%2FPvL5%2B%2FXrm3Mzu%2B8aNG4rFYurs7FQoFOIBACuI5%2F9KUO6V2%2Ftz6v3kfiafr%2FmLGebI7QG7fb13K6cedG7vM7d9ucMcc9uXuyx3%2B7nnqFTncKEertP6i%2B1Fu20%2Fd%2Binu7vb879bqjBFD3qFuXPnznN7P07P5hsfH1cqlXJdZ%2B5DXH0%2Bn3bt2pVZVlNTk%2FVIpp6enqU0PSMej%2BvmzZuLal9bW1umR7hly5Zl3ZKzGOcwGAxq69atC%2B4j95tK7reEpXr8%2BLESiUTmdVNTk2zb1vXr19XW1saFwRWMgH4B5AbH5OTkcwOsqqoqaxgj9yGuc4c5coc3csN1sRKJhB4%2Ffpz1Xm7IPI9lWWpsbFRHR4du3brl%2BjTsfBXiHM4VDofnzaR49OhRVjsXWn8xksmkTpw4Me%2F9H%2F%2F4xzp27Jju3bunVCql%2Fv7%2BZc0iQekR0CvMvXv3irbtuT3jjRs3qqamZt74Zj69z2KZnfaWSqX06NEjDQwM6PDhwws%2BcNVJMc7h%2BvXrtWbNmkWt4%2Ff7tXbt2oLs%2F%2Fz583rvvfdcn9zi8%2Fm0e%2FduffHFF%2Bru7i7IPlF8BDQybt68mXnK9uwwR6GHN5aqoaFBDx480OHDhzO9%2BRs3bujo0aP60Y9%2BpJGREU%2FaZZI%2F%2FvGPCgQCOnjw4ILno6mpSZFIpIQtw1IR0MgYGxvTnTt3Mq%2Fr6%2BvV1NSUCcSRkRH19%2FeXvF2WZenYsWNZ7aioqNBPfvITRaPRon6ryFfuEMmVK1cyY%2BZu9corrxT820gymVRXV5e2bNmisrIybdq0SUePHs0a%2B5bmz0CBmQhoZMkd5pg7bezatWuePC8vtxefOz68du3arGf8eSH3H7d8pvqVwv379xWNRvXOO%2B8s%2BQnj8A4BjSy5wxyzwZdOpzUwMOBl0zJCoZCam5slSXV1dbp06ZIqKys9blX2P26VlZW6dOlS5qKcZVnq6upSPB5Xf3%2B%2FXn%2F99YLuu7GxUWfOnMlsf%2B7MDcuy9NZbb2W%2BgUjFvZaBwuGp3sgy2xOc23OWnKedlcrs9LTZGRA%2Bn0%2BnT5%2FW6dOnHT%2B%2FZs0arV%2B%2FvuQXM%2Fv7%2BzUyMpJpZ21trb744ot5n9u9e7d2796turo6tbW1LXu%2FkUhEH3%2F88bztuynED41QGvSgMY%2FThUCvhjekb6eRuX1FT6fT%2BuCDD7LmcVdVVZWyiZJm2vmzn%2F0srwuWFy9eLEg4SzMzOFpbW%2FMawkin0zpw4AC%2FJFwhCGjMM9sTnGVCj%2Bv8%2BfOqr6%2FPalc6ndbVq1dVX1%2BvkydPZl0I8%2BoiWDKZ1JYtWxxnUiQSCV29elXbt2%2FXG2%2B8UdD9dnR0KBAIqLOzU6Ojo%2FOWj46OqrOzU4FAQIODgwXdN4rL858zUmZV7k%2BHF%2FvTboqiClP0oJHFsiydPHky66KbV3OfgZcdFwkhy7L02WefOf5c2qu5zwAYg4Zmxk0nJyfnvZ9Op3XixAnPLg4CLzt60JCkeTcqGhkZ0dtvv63bt2971CIAPNUbAAzFEAcAGIqABgBDlds2IxwAYCJ60ABgKAIaAAxFQAOAoQhoADAUAQ0AhiKgAcBQBDQAGIqABgBDEdAAYCgCGgAMRUADgKEIaAAwFAENAIYioAHAUAQ0ABiKgAYAQxHQAGAoAhoADEVAA4ChCGgAMBQBDQCGIqABwFAENAAYioAGAEMR0ABgKAIaAAxFQAOAoQhoADAUAQ0AhiKgAcBQBDQAGIqABgBDEdAAYCgCGgAMRUADgKEIaAAwFAENAIYioAHAUAQ0ABiKgAYAQxHQAGAoAhoADEVAA4ChCGgAMBQBDQCGIqABwFAENAAYioAGAEMR0ABgKAIaAAxFQAOAoQhoADAUAQ0AhiKgAcBQBDQAGIqABgBDEdAAYCgCGgAMRUADgKEIaAAwFAENAIYioAHAUAQ0ABiKgAYAQxHQAGAoAhoADEVAA4ChCGgAMBQBDQCGIqABwFAENAAYioAGAEMR0ABgKAIaAAxFQAOAoQhoADAUAQ0AhiKgAcBQBDQAGIqABgBDEdAAYCgCGgAMRUADgKEIaAAwFAENAIYioAHAUAQ0ABiKgAYAQxHQAGAoAhoADEVAA4ChCGgAMBQBDQCGIqABwFAENAAYioAGAEMR0ABgKAIaAAxFQAOAoQhoADAUAQ0AhiKgAcBQBDQAGIqABgBDEdAAYCgCGgAMRUADgKEIaAAwFAENAIYioAHAUAQ0ABiKgAYAQxHQAGAoAhoADEVAA4ChCGgAMBQBDQCGIqABwFAENAAYioAGAEMR0ABgKAIaAAxFQAOAoQhoADAUAQ0AhiKgAcBQBDQAGIqABgBDEdAAYCgCGgAMRUADgKEIaAAwFAENAIYqa2lpsU%2BcOOF1OwAAc7S0tGjVunXrvG4HACDHunXrtGrDhg1etwMAkGPDhg0q%2B%2Frrr%2B3vfve7evbsmdftAQBIKi8v19dff61V3%2FnOdxQKhbxuDwDg%2F0KhkHw%2B38wsjl%2F84hdetwcA8H979%2B6VJJXZtm1PTEzoe9%2F7nsdNAgBI0n%2F%2F%2B19VV1fP9KCrq6u1bds2r9sEAC%2B9HTt2qLq6WtKcH6r86le%2F8qxBAIAZr7%2F%2Beua%2Fy2zbtiXp6dOn%2BsEPfqAHDx541S4AeKn5%2FX6Nj49r9erVkub0oFevXq0%2F%2FOEPnjUMAF52f%2FrTnzLhLM3pQc%2Bqra3V3bt3S94wAHiZbd68WSMjIyovL8%2B8N%2B9mSZ2dnSVtFABA%2BuCDD7LCWZJkOwiHw7YkiqIoqgQVCoWcotieN8QhSYlEQrW1tUokErmLAAAF5Pf7devWLQUCgXnLHO8H7ff71dfXlzVYDQAorPLycvX09DiGs7TADfu3bdumrq6uYrULAF56f%2FnLX7Rz5073DzgOfMzR3t7u%2BfgMRVHUi1YtLS3Pi1%2F7uQE9PT1t79u3z%2FODoSiKelFq37599vT09PIDejakW1paPD8oiqKolV4tLS15hXPeAT2rq6vLLi8v9%2FwAKYqiVlqVl5fbXV1di4ncxQW0bdv2jRs3bL%2Ff7%2FnBUhRFrZTy%2B%2F32jRs3Fhu3iw9o27bt8fFxOxQKeX7QFEVRplcoFLLHx8eXErVLC%2BhZn376qb1582bPTwBFUZRptXnzZvvTTz9dTsQuL6Bte%2BYC4tmzZxn2oCiK0sxwxtmzZ%2FO%2BEFjUgJ715MkTu7Oz0962bZvnJ4iiKKrUtWPHDruzs9N%2B8uRJoWLV%2BV4cyzUxMaHe3l5dvnxZQ0NDevbsWaF3AQCeKi8vVygU0t69exUOhzOPqSqk%2FwE4qYS0U9jU7gAAAABJRU5ErkJggg%3D%3D%5Ct65%2C117%2C8%2CYour%20Name)

Both firmware and hardware are open source and licensed under the GPLv3 license.
The PCB is made by [Eurolan](http://www.eurolan.net/), you can find more details about the hardware design in the [hardware](/hardware) directory.

# Usage

`ggtag` can be programmed either with sound or with USB serial.
To program with sound, you need to put a CR2032 battery in the battery holder and flip the switch to the `BATT` position.
To program with USB serial, you need to connect `ggtag` to a USB host and flip the switch to the `USB` position.
Both programming modes are supported in the web interface hosted at [ggtag.io](https://ggtag.io), all you need is a modern HTML browser.

## Programming with sound

Here is a demonstration of how programming with sound works. Initially, three ggtags are listening and then programmed simultaneously using mobile phone.
After the initial programming, you need to press the button on the side for ggtag to start listening again. At the end make sure to flip the switch back to `USB` position in order to preserve the battery.

https://user-images.githubusercontent.com/271616/233822561-fff766fd-9242-4774-a4a4-4f87c363f370.mp4

## Programming with USB serial

The web interface is using [Web Serial API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API) for USB serial programming, so you need a web browser (e.g. Chrome, Edge) which supports this API. Mobile Chrome running on Android is also supported. Here is a demo of using Pixel5 phone for USB serial programming:

https://github.com/rgerganov/ggtag/assets/271616/d471cdec-4e41-4382-ae9f-f745ea71745e

## Emulating RFID tags

You can emulate 125kHz RFID tags with ggtag. This is accomplished with an ATtiny85 MCU and the [avrfid](https://github.com/scanlime/navi-misc/blob/master/avrfid/avrfid.S) firmware. When ggtag receives an RFID command, it generates the corresponding avrfid firmware and programs the ATtiny85 with it. Demo with Flipper Zero:

https://github.com/rgerganov/ggtag/assets/271616/654c1e1a-a8cb-4189-a39e-80669b1f9637

Another demo with USB RFID reader:

https://github.com/rgerganov/ggtag/assets/271616/f1d7bd59-880e-44ea-8732-c919e1119cd3

## Web receiver

There is a web audio receiver which implements the [ggtag protocol](https://github.com/rgerganov/ggtag/blob/master/protocol.md) running at [https://ggtag.io/recv/](https://ggtag.io/recv/).
This could be useful if you want to hack on the protocol and debug stuff directly in the browser.

https://github.com/rgerganov/ggtag/assets/271616/a54e0393-90e2-4561-994b-85e5b82250ba

## Python examples

There is a `ggtag` python package which can be used together with `pyserial` or `ggwave` to program the device. See the [examples](examples/) folder for more details.

# Known issues

 * If you switch off the battery after successful sound programming, you need to make ~5sec pause before switching it on again.
 * With some RFID readers you need to keep `ggtag` at 1cm distance for read to be successful.
 * If you want to display non-ASCII text, you need to convert it to image first.

# Credits

* [ggwave](https://github.com/ggerganov/ggwave) (MIT license)
* [microphone-library-for-pico](https://github.com/ArmDeveloperEcosystem/microphone-library-for-pico/) (Apache 2.0 license)
* [Pico_ePaper_Code](https://github.com/waveshare/Pico_ePaper_Code) (GPLv3 license)
* [QR-Code-generator](https://github.com/nayuki/QR-Code-generator) (MIT license)
* [Serial API Polyfill](https://github.com/google/web-serial-polyfill) (Apache 2.0 license)
* [avrfid](https://github.com/scanlime/navi-misc/blob/master/avrfid/avrfid.S) (BSD license)
* [stb_truetype](https://github.com/nothings/stb/blob/master/stb_truetype.h) (MIT license)
* [Font Awesome](https://fontawesome.com/) (CC BY 3.0 license)

# Project structure

The codebase is structured in the following way:
 * `target` - contains device only source code
 * `shared` - contains shared source code between the device and the host library
 * `host` - contains the host library
 * `docs` - contains the web interface
 * `python` - contains Python bindings for the host library
 * `examples` - contains some examples
 * `hardware` - contains hardware design files

## Component diagram

![component diagram](pics/diagram.png)

## Building the firmware

Follow these steps to build the RP2040 firmware:

```
git clone https://github.com/raspberrypi/pico-sdk
cd pico-sdk
git submodule init
git submodule update
cd ..
git clone https://github.com/raspberrypi/pico-extras
cd pico-extras
git submodule init
git submodule update
cd ..
export PICO_SDK_PATH=<full_path_to_pico_sdk>
export PICO_EXTRAS_PATH=<full_path_to_pico_extras>
export PICO_TOOLCHAIN_PATH=<full_path_to_arm_toolchain>

git clone git@github.com:rgerganov/ggtag
cd ggtag
git submodule init
git submodule update
mkdir build
cd build
cmake .. -DPICO_BOARD=pico
make
```

## Building the web interface

The host library is compiled to WASM using [Emscripten](https://emscripten.org/):
```
source <path_to_emsdk_env.sh>
CXX=emcc make
```
You can start a local server with `make server` and access the web interface at [http://localhost:8000](http://localhost:8000).
There is a hosted version available at [https://ggtag.io](https://ggtag.io)

## Flashing the firmware

Put ggtag into USB mode, press and hold the button and plug the USB cable.
Download the latest firmware [release](https://github.com/rgerganov/ggtag/releases) and copy the `.uf2` file to the `RPI-RP2` drive.
The tag will reboot and run the new firmware.

# Feedback

We'd love to hear your feedback, comments and questions about ggtag in the [Project Discussions](https://github.com/rgerganov/ggtag/discussions)
