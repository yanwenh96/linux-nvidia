/*
 * MC StreamID configuration
 *
 * Copyright (c) 2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define pr_fmt(fmt)	"%s(): " fmt, __func__

#include <linux/err.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>

#include <dt-bindings/memory/tegra-swgroup.h>

#define SCEW_STREAMID_WRITE_ACCESS	BIT(16)
#define SCEW_STREAMID_OVERRIDE		BIT(8)
#define SCEW_NS				BIT(0)

#define MC_SMMU_BYPASS_CONFIG_0		0x1820
#define TBU_BYPASS_SID			2

/* FIXME: Move to dt-bindings/memory/tegra-swgroup.h */
#define TEGRA_SWGROUP_SCE		52

enum override_id {
	PTCR,
	AFIR,
	HDAR,
	HOST1XDMAR,
	NVENCSRD,
	SATAR,
	MPCORER,
	NVENCSWR,
	AFIW,
	HDAW,
	MPCOREW,
	SATAW,
	ISPRA,
	ISPWA,
	ISPWB,
	XUSB_HOSTR,
	XUSB_HOSTW,
	XUSB_DEVR,
	XUSB_DEVW,
	TSECSRD,
	TSECSWR,
	GPUSRD,
	GPUSWR,
	SDMMCRA,
	SDMMCRAA,
	SDMMCR,
	SDMMCRAB,
	SDMMCWA,
	SDMMCWAA,
	SDMMCW,
	SDMMCWAB,
	VICSRD,
	VICSWR,
	VIW,
	NVDECSRD,
	NVDECSWR,
	APER,
	APEW,
	NVJPGSRD,
	NVJPGSWR,
	SESRD,
	SESWR,
	ETRR,
	ETRW,
	TSECSRDB,
	TSECSWRB,
	AXISR,
	AXISW,
	EQOSR,
	EQOSW,
	UFSHCR,
	UFSHCW,
	NVDISPLAYR,
	BPMPR,
	BPMPW,
	BPMPDMAR,
	BPMPDMAW,
	AONR,
	AONW,
	AONDMAR,
	AONDMAW,
	SCER,
	SCEW,
	SCEDMAR,
	SCEDMAW,
	APEDMAR,
	APEDMAW,
	MAX_OID,
};

static int sid_override_offset[] = {
	[PTCR]		= 0x000,
	[AFIR]		= 0x038,
	[HDAR]		= 0x054,
	[HOST1XDMAR]	= 0x058,
	[NVENCSRD]	= 0x070,
	[SATAR]		= 0x07c,
	[MPCORER]	= 0x09c,
	[NVENCSWR]	= 0x0ac,
	[AFIW]		= 0x0c4,
	[HDAW]		= 0x0d4,
	[MPCOREW]	= 0x0e4,
	[SATAW]		= 0x0f4,
	[ISPRA]		= 0x110,
	[ISPWA]		= 0x118,
	[ISPWB]		= 0x11c,
	[XUSB_HOSTR]	= 0x128,
	[XUSB_HOSTW]	= 0x12c,
	[XUSB_DEVR]	= 0x130,
	[XUSB_DEVW]	= 0x134,
	[TSECSRD]	= 0x150,
	[TSECSWR]	= 0x154,
	[GPUSRD]	= 0x160,
	[GPUSWR]	= 0x164,
	[SDMMCRA]	= 0x180,
	[SDMMCRAA]	= 0x184,
	[SDMMCR]	= 0x188,
	[SDMMCRAB]	= 0x18c,
	[SDMMCWA]	= 0x190,
	[SDMMCWAA]	= 0x194,
	[SDMMCW]	= 0x198,
	[SDMMCWAB]	= 0x19c,
	[VICSRD]	= 0x1b0,
	[VICSWR]	= 0x1b4,
	[VIW]		= 0x1c8,
	[NVDECSRD]	= 0x1e0,
	[NVDECSWR]	= 0x1e4,
	[APER]		= 0x1e8,
	[APEW]		= 0x1ec,
	[NVJPGSRD]	= 0x1f8,
	[NVJPGSWR]	= 0x1fc,
	[SESRD]		= 0x200,
	[SESWR]		= 0x204,
	[ETRR]		= 0x210,
	[ETRW]		= 0x214,
	[TSECSRDB]	= 0x218,
	[TSECSWRB]	= 0x21c,
	[AXISR]		= 0x230,
	[AXISW]		= 0x234,
	[EQOSR]		= 0x238,
	[EQOSW]		= 0x23c,
	[UFSHCR]	= 0x240,
	[UFSHCW]	= 0x244,
	[NVDISPLAYR]	= 0x248,
	[BPMPR]		= 0x24c,
	[BPMPW]		= 0x250,
	[BPMPDMAR]	= 0x254,
	[BPMPDMAW]	= 0x258,
	[AONR]		= 0x25c,
	[AONW]		= 0x260,
	[AONDMAR]	= 0x264,
	[AONDMAW]	= 0x268,
	[SCER]		= 0x26c,
	[SCEW]		= 0x270,
	[SCEDMAR]	= 0x274,
	[SCEDMAW]	= 0x278,
	[APEDMAR]	= 0x27c,
	[APEDMAW]	= 0x280,
};

#define MAX_OIDS_IN_SID 5
struct sid_to_oids
{
	int sid;			/* StreamID */
	int noids;			/* # of override IDs */
	int oid[MAX_OIDS_IN_SID];	/* Override IDs */
};

static struct sid_to_oids sid_to_oids[] = {
	{
		.sid	= TEGRA_SWGROUP_AFI,
		.noids	= 2,
		.oid	= {
			AFIR,
			AFIW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_HDA,
		.noids	= 2,
		.oid	= {
			HDAR,
			HDAW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_SATA2,
		.noids	= 2,
		.oid	= {
			SATAR,
			SATAW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_XUSB_HOST,
		.noids	= 2,
		.oid	= {
			XUSB_HOSTR,
			XUSB_HOSTW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_XUSB_DEV,
		.noids	= 2,
		.oid	= {
			XUSB_DEVR,
			XUSB_DEVW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_TSEC,
		.noids	= 2,
		.oid	= {
			TSECSRD,
			TSECSWR,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_GPUB,
		.noids	= 2,
		.oid	= {
			GPUSRD,
			GPUSWR,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_SDMMC1A,
		.noids	= 2,
		.oid	= {
			SDMMCRA,
			SDMMCWA,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_SDMMC2A,
		.noids	= 2,
		.oid	= {
			SDMMCRAA,
			SDMMCWAA,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_SDMMC3A,
		.noids	= 2,
		.oid	= {
			SDMMCR,
			SDMMCW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_SDMMC4A,
		.noids	= 2,
		.oid	= {
			SDMMCRAB,
			SDMMCWAB,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_APE,
		.noids	= 4,
		.oid	= {
			APER,
			APEW,
			APEDMAR,
			APEDMAW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_SE,
		.noids	= 2,
		.oid	= {
			SESRD,
			SESWR,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_ETR,
		.noids	= 2,
		.oid	= {
			ETRR,
			ETRW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_TSECB,
		.noids	= 2,
		.oid	= {
			TSECSRDB,
			TSECSWRB,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_AXIS,
		.noids	= 2,
		.oid	= {
			AXISR,
			AXISW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_EQOS,
		.noids	= 2,
		.oid	= {
			EQOSR,
			EQOSW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_UFSHC,
		.noids	= 2,
		.oid	= {
			UFSHCR,
			UFSHCW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_NVDISPLAY,
		.noids	= 1,
		.oid	= {
			NVDISPLAYR,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_BPMP,
		.noids	= 4,
		.oid	= {
			BPMPR,
			BPMPW,
			BPMPDMAR,
			BPMPDMAW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_AON,
		.noids	= 4,
		.oid	= {
			AONR,
			AONW,
			AONDMAR,
			AONDMAW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_SCE,
		.noids	= 4,
		.oid	= {
			SCER,
			SCEW,
			SCEDMAR,
			SCEDMAW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_HC,
		.noids	= 1,
		.oid	= {
			HOST1XDMAR,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_VIC,
		.noids	= 2,
		.oid = {
			VICSRD,
			VICSWR,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_VI,
		.noids	= 1,
		.oid	= {
			VIW,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_ISP,
		.noids	= 3,
		.oid	= {
			ISPRA,
			ISPWA,
			ISPWB,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_NVDEC,
		.noids	= 2,
		.oid	= {
			NVDECSRD,
			NVDECSWR,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_NVENC,
		.noids	= 2,
		.oid	= {
			NVENCSRD,
			NVENCSWR,
		},
	},
	{
		.sid	= TEGRA_SWGROUP_NVJPG,
		.noids	= 2,
		.oid	= {
			NVJPGSRD,
			NVJPGSWR,
		},
	},
};

static void __iomem *mc_sid_base;

static struct of_device_id mc_sid_of_match[] = {
	{ .compatible = "nvidia,tegra-mc-sid", },
}
MODULE_DEVICE_TABLE(of, mc_sid_of_match);

static void __mc_override_sid(int sid, int oid)
{
	volatile void __iomem *addr;
	u32 val;

	BUG_ON(oid >= MAX_OID);

#if 0	/* FIXME: wait for linsim update */
	addr = mc_sid_base + sid_override_offset[oid];
	addr += sizeof(u32); /* MC_SID_STREAMID_SECURITY_CONFIG_* */
	val = SCEW_STREAMID_WRITE_ACCESS | SCEW_STREAMID_OVERRIDE | SCEW_NS;
	writel_relaxed(val, addr);

	addr = mc_sid_base + sid_override_offset[oid];
	writel_relaxed(sid, addr);
#else
	addr = mc_sid_base + sid_override_offset[oid];
	val = 0x80010000 | sid;
	writel_relaxed(val, addr);
#endif
	pr_debug("override sid=%d oid=%d at offset=%x\n",
		 sid, oid, sid_override_offset[oid]);
}

void platform_override_streamid(int sid)
{
	int i;

	if (!mc_sid_base) {
		pr_err("mc-sid isn't populated\n");
		return;
	}

	for (i = 0; i < ARRAY_SIZE(sid_to_oids); i++) {
		struct sid_to_oids *conf;

		conf = &sid_to_oids[i];
		BUG_ON(conf->noids >= MAX_OIDS_IN_SID);
		if (sid == conf->sid) {
			int j;

			for (j = 0; j < conf->noids; j++) {
				int oid = conf->oid[j];

				__mc_override_sid(sid, oid);
			}
			return;
		}
	}
}

static int mc_sid_probe(struct platform_device *pdev)
{
	int i;
	struct resource *res;
	static void __iomem *addr;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	addr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(addr))
		return PTR_ERR(addr);
	mc_sid_base = addr;

	for (i = 0; i < ARRAY_SIZE(sid_override_offset); i++)
		__mc_override_sid(0x7f, i);

	/* FIXME: wait for MC driver */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	addr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(addr))
		return PTR_ERR(addr);

	writel_relaxed(TBU_BYPASS_SID, addr + MC_SMMU_BYPASS_CONFIG_0);
	return 0;
}

static struct platform_driver mc_sid_driver = {
	.probe	= mc_sid_probe,
	.driver	= {
		.owner	= THIS_MODULE,
		.name	= "mc-sid",
		.of_match_table	= of_match_ptr(mc_sid_of_match),
	},
};

static int __init mc_sid_init(void)
{
	return platform_driver_register(&mc_sid_driver);
}
arch_initcall_sync(mc_sid_init); /* FIXME: population order */

MODULE_DESCRIPTION("MC StreamID configuration");
MODULE_AUTHOR("Hiroshi DOYU <hdoyu@nvidia.com>");
MODULE_LICENSE("GPL v2");
